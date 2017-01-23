using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;

// MME-BOOT - Complete CAN BUS (MCP2515) XMega Bootloader with AES 
// By Marko Mlakar (marko@mlakar.si)
// 
// https://community.atmel.com/projects/mme-boot-complete-can-bus-mcp2515-xmega-bootloader-aes-and-software-encrypt-update
// 


namespace UpgradeTool
{
    public partial class UpgradeForm : Form
    {
        public UpgradeForm()
        {
            InitializeComponent();
        }

        public delegate void SendPacketDelegate(int id, byte dlc, byte[] data);
        public SendPacketDelegate SendPacket = null;

        MicroTimer BulkPageTransferTimer = new MicroTimer();

        enum PhaseTypes { None, WaitingForBootloader, WaitingForFirmwareSelection, WaitingForWriteCommand, WritingFirmware, ConnectingToDevice }
        PhaseTypes interfacePhase = PhaseTypes.None;
        enum FirmwareErrors { None, Corrupted, InvalidPageSize, CrcMissmatch, InvalidSignature };

        int writingPage = 0;
        int writingDataIndex = 0;
        byte writingExpectingSequence = 0;

        int newFirmwarePageSize = 0;
        List<byte[]> newfirmwarePages = new List<byte[]>();
        byte[] newFirmwareVersion = null;
        byte fileDeviceId = 0;
        byte fileDeviceCode = 0;
        List<byte[]> fileAllowedVersions = null;

        byte currentDeviceId = 0;
        byte[] currentFirmware = new byte[4];
        
        const int PC_AVR_CAN_ID = 0x7EF;
        const int AVR_PC_CAN_ID = 0x07FF;

        bool bulkTransfer = true;
        int bulkTransferInterval = 5000; // 3000 microseconds = 3ms

        bool skipOpenFile = false;

        byte[] _ReadAllBytes(string file)
        {
            byte[] oFileBytes = null;
            using (System.IO.FileStream fs = System.IO.File.Open(file, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.ReadWrite))
            {
                int numBytesToRead = Convert.ToInt32(fs.Length);
                oFileBytes = new byte[(numBytesToRead)];
                fs.Read(oFileBytes, 0, numBytesToRead);
            }

            return oFileBytes;
        }

        bool OpenFirmwareFile(string file, ref byte deviceId, ref byte deviceCode, ref byte[] version, ref List<byte[]> allowedVersions, ref int pageSize, ref List<byte[]> pages, ref FirmwareErrors error)
        {
            // File structure:
            // 0-2      file signature (3 bytes)
            // 3        device id (1 byte)
            // 4        device code (1 byte)
            // 5-8      firmware version (4 bytes)
            // 9-408    allowed versions (100 x 4 bytes)
            // 409-410  page size [H][L] (2 bytes)
            // 411      data crc (1 byte)
            // 412-N    data (N bytes)

            byte[] content = _ReadAllBytes(file);

            if (content.Length > 411)
            {
                if (content[0] == 0x13 && content[1] == 0x22 && content[2] == 0x31)
                {
                    deviceId = content[3];
                    deviceCode = content[4];

                    version = new byte[4];
                    Array.Copy(content, 5, version, 0, version.Length);

                    allowedVersions = new List<byte[]>();
                    for (int x = 0; x < 400; x += 4)
                    {
                        allowedVersions.Add(new byte[4]);
                        Array.Copy(content, 9 + x, allowedVersions[allowedVersions.Count - 1], 0, 4);
                    }

                    pageSize = (content[409] << 8) + content[410];
                    if (pageSize == 256 || pageSize == 512 || pageSize == 128)
                    {
                        byte expectedCrc = content[411];

                        byte crc = 0;

                        int i = 412;
                        pages = new List<byte[]>();
                        while (i <= content.Length - pageSize)
                        {
                            pages.Add(new byte[pageSize]);
                            Array.Copy(content, i, pages[pages.Count - 1], 0, pageSize);
                            i += pageSize;

                            foreach (byte b in pages[pages.Count - 1])
                                crc += b;
                        }

                        if (crc != expectedCrc)
                        {
                            error = FirmwareErrors.CrcMissmatch;
                            return false;
                        }

                        return true;
                    }
                    else
                    {
                        error = FirmwareErrors.InvalidPageSize;
                        return false;
                    }

                }
                else
                {
                    error = FirmwareErrors.InvalidSignature;
                    return false;
                }
            }
            else
            {
                error = FirmwareErrors.Corrupted;
                return false;
            }
        }

        bool FirmwareIsAllowedToUpgradeTo(byte[] newVersion, byte deviceId, byte[] currentVersion, List<byte[]> allowedVersions, ref string reason)
        {
            return true;

            if (newVersion[0] == currentVersion[0] && newVersion[1] == currentVersion[1] && newVersion[2] == currentVersion[2] && newVersion[3] == currentVersion[3])
            {
                reason = "Device is already using this firmware version (" + FirmwareToString(deviceId, newVersion) + ")";
                return false;
            }

            if (allowedVersions != null)
            {
                bool atleastOneNotEmpty = false;
                foreach(byte[] a in allowedVersions)
                {
                    if (a[0] != 0 || a[1] != 0 || a[2] != 0 || a[3] != 0)
                    {
                        atleastOneNotEmpty = true;
                        if (a[0] == currentVersion[0] && a[1] == currentVersion[1] && a[2] == currentVersion[2] && a[3] == currentVersion[3])
                        {
                            return true;
                        }
                    }
                }
                if (atleastOneNotEmpty)
                {
                    reason = "Selected firmware (" + FirmwareToString(deviceId, newVersion) + " is not allowed to be upgraded from " + FirmwareToString(deviceId, currentVersion);
                    return false;
                }
            }
            return true;
        }
        string FirmwareToString(byte device, byte[] firmware)
        {
            switch (device)
            {
                case 0x50:
                    if (firmware.Length > 2)
                    {
                        return (2000 + firmware[0]) + "-" + (firmware[1] < 10 ? "0" + firmware[1] : firmware[1].ToString()) + "-" + (firmware[2] < 10 ? "0" + firmware[2] : firmware[2].ToString());
                    }
                    break;
            }
            return "(unknown)";
        }
        string DeviceIdToString(byte id)
        {
            switch (id)
            {
                case 0x50:
                    return "GCU7";

                default:
                    return "(Unknown)";
            }
        }

        void ActionButton_Click(object sender, EventArgs e)
        {
            switch ((string)ActionButton.Tag)
            {
                case "0":
                    MessageLabel.Text = "Please open new firmware file";

                    ActionButton.Enabled = true;
                    ActionButton.Text = "OPEN FILE";
                    ActionButton.Tag = "1";
                    skipOpenFile = false;
                    break;

                case "1": // open file
                    if (skipOpenFile || (OpenFirmwareFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK))
                    {
                        FirmwareErrors err = FirmwareErrors.None;
                        
                        if (OpenFirmwareFile(OpenFirmwareFileDialog.FileName, ref fileDeviceId, ref fileDeviceCode, ref newFirmwareVersion, ref fileAllowedVersions, ref newFirmwarePageSize, ref newfirmwarePages, ref err))
                        {
                            FirmwareDeviceNameLabel.Text = DeviceIdToString(fileDeviceId);
                            FirmwareVersionLabel.Text = FirmwareToString(fileDeviceId, newFirmwareVersion);

                            if (!skipOpenFile)
                            {
                                StatusMessageLabel.Text = "Firmware file successfully loaded.";
                                StatusMessageLabel.ForeColor = Color.Green;
                                StatusMessageLabel.Visible = true;
                            }

                            MessageLabel.Text = "Click connect to device to continue. Nothing will be written yet.";

                            ActionButton.Text = "CONNECT TO DEVICE";
                            ActionButton.Tag = "2";
                            ActionButton.Enabled = true;

                            interfacePhase = PhaseTypes.ConnectingToDevice;
                        }
                        else
                        {
                            switch (err)
                            {
                                case FirmwareErrors.Corrupted:
                                case FirmwareErrors.CrcMissmatch:
                                    MessageBox.Show("Firmware file looks corrupted.");
                                    break;

                                case FirmwareErrors.InvalidPageSize:
                                    MessageBox.Show("Firmware file has incorrect PageSize defined. Are you sure you're using correct firmware file?");
                                    break;

                                case FirmwareErrors.InvalidSignature:
                                    MessageBox.Show("Firmware file is not recognized.");
                                    break;

                                default:
                                    MessageBox.Show("Unknown error (" + (int)err + ")");
                                    break;
                            }
                        }
                    }
                    break;

                case "2": // read firmware version
                    MessageLabel.Text = "Turn device OFF and ON.";

                    UpgradeProgressBar.Minimum = 0;
                    UpgradeProgressBar.Maximum = 800; // timer triggers every 10ms so 800*10 = 8s max wait time
                    UpgradeProgressBar.Value = 0;

                    ActionButton.Text = "CONNECTING TO DEVICE ...";
                    ActionButton.Enabled = false;

                    UpgradeTimer.Interval = 10;
                    UpgradeTimer.Enabled = true;
                    interfacePhase = PhaseTypes.WaitingForBootloader;
                    break;

                case "3": // write firmware
                    MessageLabel.Text = "Click WRITE NEW FIRMWARE to upgrade device " + FirmwareDeviceNameLabel.Text;
                    UpgradeTimer.Enabled = false;
                    interfacePhase = PhaseTypes.WaitingForWriteCommand;

                    ActionButton.Enabled = true;
                    ActionButton.Tag = "4";
                    ActionButton.Text = "WRITE NEW FIRMWARE";
                    break;

                case "4": // WRITING FIRMWARE
                    writingPage = 0;

                    // ERASE_FIRMWARE(0x1), returns 0x1 <success[1]>
                    SendPacket(PC_AVR_CAN_ID, 1, new byte[] { 0x1 }); // erase flash. Everything else from there on is handled in the can read event.

                    ActionButton.Tag = "5";
                    ActionButton.Text = "WRITING TO DEVICE";
                    ActionButton.Enabled = false;

                    UpgradeProgressBar.Minimum = 1;
                    UpgradeProgressBar.Maximum = newfirmwarePages.Count;
                    UpgradeProgressBar.Value = 1;

                    interfacePhase = PhaseTypes.WritingFirmware;
                    UpgradeTimer.Enabled = true;
                    UpgradeTimer.Interval = 1000;
                    break;
            }
        }

        private void UpgradeTimer_Tick(object sender, EventArgs e)
        {
            UpgradeTimer.Enabled = false;

            switch (interfacePhase)
            {
                case PhaseTypes.WaitingForBootloader:
                    if (UpgradeProgressBar.Value < UpgradeProgressBar.Maximum)
                    {
                        UpgradeTimer.Enabled = true;
                        SendPacket(PC_AVR_CAN_ID, 3, new byte[] { 0x99, fileDeviceId, fileDeviceCode }); // LOGIN(0x99) <device_id> <code>

                        ++UpgradeProgressBar.Value; --UpgradeProgressBar.Value; ++UpgradeProgressBar.Value;
                    }
                    else
                    {
                        StatusMessageLabel.Text = "Failed to read device firmware. Please try again.";
                        StatusMessageLabel.ForeColor = Color.Red;
                        StatusMessageLabel.Visible = true;

                        UpgradeProgressBar.Value = 0;

                        UpgradeTimer.Enabled = false;

                        skipOpenFile = true;

                        ActionButton.Tag = "1";
                        ActionButton_Click(null, null);
                    }
                    break;

                case PhaseTypes.WritingFirmware:
                    Output.AppendText("Timeout while writing/uploading page " + writingPage + "\r\n");

                    // SET_PAGE(0x2) <pageH> <pageL> (first page = 0), returns 0x2 <success[1]> <pageH> <pageL>
                    SendPacket(PC_AVR_CAN_ID, 3, new byte[] { 0x2, (byte)(writingPage >> 8), (byte)writingPage });

                    Output.AppendText("Uploading page " + writingPage + "\r\n");

                    UpgradeTimer.Enabled = true;
                    break;
            }
        }

        private void ActionButton_TextChanged(object sender, EventArgs e)
        {
            // ActionButton.Left = (this.Width - ActionButton.Width) / 2;
        }

        private void UpgradeForm_Load(object sender, EventArgs e)
        {
            StatusMessageLabel.Text = "";

            ActionButton.Tag = "0";
            ActionButton_Click(null, null);

            ActionButton_TextChanged(ActionButton, null); // center the button

            BulkPageTransferTimer.MicroTimerElapsed += BulkPageTransferTimer_Tick;
            BulkPageTransferTimer.IgnoreEventIfLateBy = 5000;
            BulkPageTransferTimer.Stop();
        }

        public void OnPacketRead(int id, byte dlc, byte[] data)
        {
            if (id == AVR_PC_CAN_ID)
            {
                switch (data[0])
                {
                    case 0x99: // LOGIN(0x99) <authorized[1/0]> <device_id> <device_firmware_valid[1/0]> <device_firmware1> <device_firmware2> <device_firmware3> <device_firmware4>
                        if (dlc == 8)
                        {
                            if (data[1] == 1) // authorization success
                            {
                                currentDeviceId = data[2];
                                currentFirmware = new byte[] { data[4], data[5], data[6], data[7] };

                                CurrentFirmwareVersionLabel.Text = FirmwareToString(currentDeviceId, currentFirmware);

                                string reason = "";
                                if (FirmwareIsAllowedToUpgradeTo(newFirmwareVersion, currentDeviceId, currentFirmware, fileAllowedVersions, ref reason))
                                {
                                    StatusMessageLabel.Text = "Successfully connected to " + FirmwareDeviceNameLabel.Text + ". Ready to upgrade.";
                                    StatusMessageLabel.ForeColor = Color.Green;
                                    StatusMessageLabel.Visible = true;

                                    UpgradeProgressBar.Value = 0;
                                    UpgradeTimer.Enabled = false;

                                    ActionButton.Tag = "3";
                                    ActionButton_Click(null, null);
                                }
                                else
                                {
                                    UpgradeProgressBar.Value = 0;
                                    UpgradeTimer.Enabled = false;

                                    StatusMessageLabel.Text = reason;
                                    StatusMessageLabel.ForeColor = Color.Red;

                                    ActionButton.Tag = "0";
                                    ActionButton_Click(null, null);

                                    MessageBox.Show(reason);
                                    return;
                                }
                            }
                            else
                            {
                                UpgradeProgressBar.Value = 0;
                                UpgradeTimer.Enabled = false;

                                StatusMessageLabel.Text = "Device found but not authorized to burn firmware";
                                StatusMessageLabel.ForeColor = Color.Red;

                                ActionButton.Tag = "0";
                                ActionButton_Click(null, null);

                                MessageBox.Show(StatusMessageLabel.Text);
                            }
                        }
                        break;

                    case 0x1: // ERASE_FIRMWARE <success[1]>
                        if (data[1] == 1)
                        {
                            // SET page 0
                            // SET_PAGE(0x2) <pageH> <pageL> (first page = 0), returns 0x2 <success[1]> <pageH> <pageL>
                            SendPacket(PC_AVR_CAN_ID, 3, new byte[] { 0x2, 0, 0 });
                            writingPage = 0;

                            Output.AppendText("Uploading page " + writingPage + "\r\n");
                        }
                        else
                        {
                            MessageBox.Show("Erase flash failed. Upgrade aborted.");
                            return;
                        }
                        break;

                    case 0x2: // SET_PAGE <success[1]> <pageH> <pageL>
                        if (data[1] == 1)
                        {
                            int p = (data[2] << 8) + data[3];
                            if (p == writingPage)
                            {
                                if (bulkTransfer)
                                {
                                    UpgradeTimer.Enabled = false;

                                    // if we're using bulkTransfer, we send first 7 bytes of data and then continue in the BulkTransferTimer_Tick
                                    // ADD_PAGE_DATA_BULK(0x33) <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>] [b7]
                                    SendPacket(PC_AVR_CAN_ID, 8, new byte[] { 0x33, newfirmwarePages[p][0], newfirmwarePages[p][1], newfirmwarePages[p][2], newfirmwarePages[p][3], newfirmwarePages[p][4], newfirmwarePages[p][5], newfirmwarePages[p][6], newfirmwarePages[p][7] });

                                    BulkPageTransferTimer.Interval = bulkTransferInterval;
                                    BulkPageTransferTimer.Start();

                                    writingDataIndex = 7;
                                }
                                else
                                {
                                    UpgradeTimer.Enabled = false; UpgradeTimer.Enabled = true; // reset timeout timer

                                    // Send first set (6 bytes) of data
                                    // ADD_PAGE_DATA(0x3) <sequence> <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>], return 0x3 <success[1/0]> <buf_indexH> <buf_indexL> <sequence + 1>
                                    SendPacket(PC_AVR_CAN_ID, 8, new byte[] { 0x3, 0, newfirmwarePages[p][0], newfirmwarePages[p][1], newfirmwarePages[p][2], newfirmwarePages[p][3], newfirmwarePages[p][4], newfirmwarePages[p][5] });

                                    writingDataIndex = 6;
                                    writingExpectingSequence = 1;
                                }
                            }
                            else
                            {

                            }
                        }
                        else
                        {

                        }
                        break;

                    case 0x3: // ADD_PAGE_DATA <success[1/0]> <buf_indexH> <buf_indexL> <sequence + 1>                        
                        if (data[1] == 1)
                        {
                            UpgradeTimer.Enabled = false; UpgradeTimer.Enabled = true;

                            byte seq = data[4];
                            int idx = (data[2] << 8) + data[3];

                            if (seq == writingExpectingSequence)
                            {
                                if (writingDataIndex < newFirmwarePageSize)
                                {
                                    byte[] b = new byte[6];

                                    byte total = 0;
                                    while (writingDataIndex < newFirmwarePageSize && total < b.Length) { b[total++] = newfirmwarePages[writingPage][writingDataIndex++]; }

                                    // ADD_PAGE_DATA(0x3) <sequence> <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>], return 0x3 <success[1/0]> <buf_indexH> <buf_indexL> <sequence + 1>
                                    SendPacket(PC_AVR_CAN_ID, (byte)(total + 2), new byte[] { 0x3, writingExpectingSequence, b[0], b[1], b[2], b[3], b[4], b[5] });
                                    writingExpectingSequence += 1;
                                }
                                else
                                {
                                    // last set received.
                                    byte crc = 0;
                                    foreach (byte b in newfirmwarePages[writingPage]) crc += b;

                                    // FLASH_LOADED_PAGE(0x4) <crc>, returns 0x4 <success[1/0]> <pageH> <pageL>
                                    SendPacket(PC_AVR_CAN_ID, 2, new byte[] { 0x4, crc });
                                }
                            }
                            else
                            {
                            }
                        }
                        else
                        {

                        }
                        break;

                    case 0x4: // FLASH_LOADED_PAGE(0x4) <success[1/0]> <pageH> <pageL>
                        if (data[1] == 1)
                        {
                            int p = (data[2] << 8) + data[3];
                            if (p == writingPage)
                            {
                                if (writingPage < newfirmwarePages.Count - 1)
                                {
                                    writingPage++;

                                    UpgradeProgressBar.Value = writingPage;

                                    // SET_PAGE(0x2) <pageH> <pageL> (first page = 0), returns 0x2 <success[1]> <pageH> <pageL>
                                    SendPacket(PC_AVR_CAN_ID, 3, new byte[] { 0x2, (byte)(writingPage >> 8), (byte)writingPage });
                                    Output.AppendText("Uploading page " + writingPage + "\r\n");

                                    writingDataIndex = 6;
                                    writingExpectingSequence = 1;
                                }
                                else
                                {
                                    Output.AppendText("Updating firmware version\r\n");
                                    // SEND NEW FIRMWARE VERSION
                                    // NEW_FIRMWARE_VERSION(0x10) <firmware1> <firmware2> <firmware3> <firmware4>, RETURNS 0x10 <success[0/1]> <firmware1> <firmware2> <firmware3> <firmware4>
                                    SendPacket(PC_AVR_CAN_ID, 5, new byte[] { 0x10, newFirmwareVersion[0], newFirmwareVersion[1], newFirmwareVersion[2], newFirmwareVersion[3] });
                                }
                            }
                            else
                            {

                            }
                        }
                        else
                        {
                            Output.AppendText("Invalid CRC for " + writingPage + ", resend.\r\n");

                            // Invalid crc for page, resend complete page.
                            SendPacket(PC_AVR_CAN_ID, 3, new byte[] { 0x2, (byte)(writingPage >> 8), (byte)writingPage });
                        }
                        break;

                    case 0x10: // NEW_FIRMWARE_VERSION(0x10) <success[0/1]> <firmware1> <firmware2> <firmware3> <firmware4>
                        if (data[1] == 1)
                        {
                            Output.AppendText("Victory!\r\n");

                            UpgradeProgressBar.Value = UpgradeProgressBar.Maximum;
                            UpgradeTimer.Enabled = false;

                            StatusMessageLabel.Text = "Firmware successfully written.";
                            StatusMessageLabel.ForeColor = Color.Green;
                            StatusMessageLabel.Visible = true;

                            SendPacket(PC_AVR_CAN_ID, 1, new byte[] { 0x20 }); Thread.Sleep(100);
                            SendPacket(PC_AVR_CAN_ID, 1, new byte[] { 0x20 }); Thread.Sleep(100);

                            ActionButton.Tag = "0";
                            ActionButton_Click(null, null);

                            MessageBox.Show("Firmware successfully written. Device should automatically start. If not, turn device off and on again.");
                        }
                        else
                        {

                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {

            }
        }

        #region Precision timer
        object timerRunningLock = new object();
        void BulkPageTransferTimer_Tick(object sender, MicroTimerEventArgs timerEventArgs)
        {
            // execute timer in main thread
            this.Invoke(new MicroTimer.MicroTimerElapsedEventHandler(BulkPageTransferTimer_TickInMainThread), new object[] { sender, timerEventArgs });
        }
        void BulkPageTransferTimer_TickInMainThread(object sender, MicroTimerEventArgs timerEventArgs)
        {
            if (Monitor.TryEnter(timerRunningLock))
            {
                try
                {
                    if (writingDataIndex < newFirmwarePageSize)
                    {
                        byte[] b = new byte[7];

                        byte total = 0;
                        while (writingDataIndex < newFirmwarePageSize && total < b.Length) { b[total++] = newfirmwarePages[writingPage][writingDataIndex++]; }

                        // ADD_PAGE_DATA_BULK(0x33) <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>] [b7]
                        SendPacket(PC_AVR_CAN_ID, (byte)(total + 1), new byte[] { 0x33, b[0], b[1], b[2], b[3], b[4], b[5], b[6] });
                    }
                    else
                    {
                        // last set received, disable BulkTransferTimer, send FLASH_LOADED_PAGE command and wait for the "successfully written" confirmation.
                        byte crc = 0;
                        foreach (byte b in newfirmwarePages[writingPage]) crc += b;

                        // FLASH_LOADED_PAGE(0x4) <crc>, returns 0x4 <success[1/0]> <pageH> <pageL>
                        SendPacket(PC_AVR_CAN_ID, 2, new byte[] { 0x4, crc });

                        UpgradeTimer.Enabled = true;

                        BulkPageTransferTimer.Stop();
                    }
                }
                finally
                {
                    Monitor.Exit(timerRunningLock);
                }
            }
        }
        #endregion
    }
}
