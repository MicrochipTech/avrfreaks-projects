using LibGIS.Net;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Windows.Forms;

// MME-BOOT - Complete CAN BUS (MCP2515) XMega Bootloader with AES 
// By Marko Mlakar (marko@mlakar.si)
// 
// https://community.atmel.com/projects/mme-boot-complete-can-bus-mcp2515-xmega-bootloader-aes-and-software-encrypt-update
// 

namespace EncryptFirmware
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        Random rnd = new Random();

        public static byte[] AESEncrypt(byte[] input, byte[] key, byte[] iv)
        {
            RijndaelManaged rDel = new RijndaelManaged();
            rDel.Key = key;
            rDel.Mode = CipherMode.CBC;
            rDel.IV = iv;
            rDel.Padding = PaddingMode.Zeros;
            ICryptoTransform cTransform = rDel.CreateEncryptor();
            return cTransform.TransformFinalBlock(input, 0, input.Length);
        }

        private void NewFirmware_TextChanged(object sender, EventArgs e)
        {            
        }

        private void SelectOldFirmwareFileButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog.Filter = "Bin files|*.bin|All files|*.*";
            if (OpenFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                OldFirmwareFile.Text = OpenFileDialog.FileName;
            }
        }
        private void SelectNewFirmwareButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog.Filter = "Hex files|*.hex|All files|*.*";
            if (OpenFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                NewFirmwareFile.Text = OpenFileDialog.FileName;
            }
        }

        private void SaveNewFirmwareFile_Click(object sender, EventArgs e)
        {
            if (NewFirmwareVersion.Text.Trim() == "")
            {
                MessageBox.Show("You need to enter new firmware version before saving.");
                return;
            }

            byte[] key = NumbersAsTextToByteArray(AESKey.Text);
            if (key.Length != 16)
            {
                MessageBox.Show("AES key is not 16 bytes long.");
                return;
            }

            byte[] iv = NumbersAsTextToByteArray(AESIV.Text);
            if (iv.Length != 16)
            {
                MessageBox.Show("AES IV is not 16 bytes long.");
                return;
            }

            // File structure:
            // 0-2      file signature (3 bytes)
            // 3        device id (1 byte)
            // 4        device code (1 byte)
            // 5-8      firmware version (4 bytes)
            // 9-408    allowed versions (100 x 4 bytes)
            // 409-410  page size [H][L] (2 bytes)
            // 411      data crc (1 byte)
            // 412-N    data (N bytes)

            byte[] header = new byte[411];
            // 3 signature bytes to recognize the format
            header[0] = 0x13;
            header[1] = 0x22;
            header[2] = 0x31;

            // device
            try
            {
                header[3] = Byte.Parse(DeviceId.Text.Trim());
            }
            catch(Exception ex)
            {
                MessageBox.Show("Invalid Device ID");
            }

            try
            {
                header[4] = Byte.Parse(DeviceCode.Text.Trim());
            }
            catch (Exception ex)
            {
                MessageBox.Show("Invalid Device Code");
            }


            // new firmware version
            if (NewFirmwareVersion.Text.Trim() == "")
            {
                MessageBox.Show("Please enter new firmware version (atleast 1 byte)");
                return;
            }
            byte[] newFirmwareVersion = NumbersAsTextToByteArray(NewFirmwareVersion.Text);
            Array.Copy(newFirmwareVersion, 0, header, 5, newFirmwareVersion.Length < 4 ? newFirmwareVersion.Length : 4); // 5-8

            byte[] allowedFirmwareVersions = new byte[400];
            if (AllowedFirmwareVersions.Text.Trim() != "")
            {
                string[] parts = AllowedFirmwareVersions.Text.Trim().Split(';');
                if (parts.Length > 100)
                {
                    MessageBox.Show("Only 100 firmware versions allowed.");
                    return;
                }
                int i = 0;
                foreach (string p in parts)
                {
                    byte[] ver = NumbersAsTextToByteArray(p);
                    Array.Copy(ver, 0, allowedFirmwareVersions, i, ver.Length <= 4 ? ver.Length : 4);
                    i += 4;
                }
            }
            Array.Copy(allowedFirmwareVersions, 0, header, 9, allowedFirmwareVersions.Length); // 9-408

            // page size
            int pageSize = DevicePageSize.SelectedIndex == 0 ? 512 : 256;
            header[409] = (byte)(pageSize >> 8);
            header[410] = (byte)(pageSize & 0xFF);

            StreamReader reader = new StreamReader(NewFirmwareFile.Text);

            IntelHex ihex = new IntelHex();
            IntelHexStructure s = new IntelHexStructure();

            List<byte[]> pages = new List<byte[]>();

            // read hex file
            byte[] page = new byte[pageSize];
            for (int b = 0; b < pageSize; b++) page[b] = 0xFF;

            int index = 0;
            int line = 0;
            while((s = ihex.Read(reader)) != null)
            {
                line++;

                if (s.type == 0) // type=0 = data
                {
                    int toAdd = index + s.dataLen < pageSize ? s.dataLen : pageSize - index;

                    Array.Copy(s.data, 0, page, index, toAdd);
                    index += toAdd;

                    // page full, add it to list
                    if (index >= pageSize)
                    {
                        pages.Add(page);

                        page = new byte[pageSize];
                        for (int b = 0; b < pageSize; b++) page[b] = 0xFF;

                        if (toAdd == 16) // some leftovers from previous page
                        {
                            index = 0;
                        }
                        else
                        {
                            Array.Copy(s.data, toAdd, page, 0, 16 - toAdd);
                            index = 16 - toAdd;
                        }
                    }
                }
            }
            if (index > 0)
                pages.Add(page);

            string outputFile = NewFirmwareFile.Text.Replace(".hex", ".bin");

            // save new file
            FileStream output = new FileStream(outputFile, FileMode.Create);
            output.Write(header, 0, header.Length);

            // encrypt pages & calculate crc
            byte crc = 0;
            for (int p = 0; p < pages.Count; p++)
            {
                pages[p] = AESEncrypt(pages[p], key, iv);
                foreach (byte b in pages[p]) crc += b;
            }

            // save crc
            output.Write(new byte[1] { crc }, 0, 1);

            // save pages
            for (int p = 0; p < pages.Count; p++)
                output.Write(pages[p], 0, pages[p].Length);

            output.Close();

            MessageBox.Show(outputFile + " successfully saved.");
        }

        byte[] NumbersAsTextToByteArray(string numbers)
        {
            string[] parts = numbers.Trim().Split(',');
            byte[] b = new byte[parts.Length];

            for (int i = 0; i < b.Length; i++)
            {
                try
                {
                    b[i] = Byte.Parse(parts[i]);
                }
                catch (Exception ex)
                { 
                    MessageBox.Show("Error converting " + parts[i] + " to byte: " + ex.Message); 
                }
            }

            return b;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void GenerateAESKey_Click(object sender, EventArgs e)
        {
            AESKey.Text = "";
            for (int x=0; x<16; x++)
            {
                if (x > 0) AESKey.Text += ", ";
                AESKey.Text += rnd.Next(0, 255).ToString();
            }
        }

        private void GenerateAESIV_Click(object sender, EventArgs e)
        {
            AESIV.Text = "";
            for (int x = 0; x < 16; x++)
            {
                if (x > 0) AESIV.Text += ", ";
                AESIV.Text += rnd.Next(0, 255).ToString();
            }
        }

        private void DeviceId_TextChanged(object sender, EventArgs e)
        {

        }

        private void ReadOldFirmwareFileButton_Click(object sender, EventArgs e)
        {
            // 0-2      file signature (3 bytes)
            // 3        device id (1 byte)
            // 4        device code (1 byte)
            // 5-8      firmware version (4 bytes)
            // 9-408    allowed versions (100 x 4 bytes)
            // 409-410  page size [H][L] (2 bytes)
            // 411      data crc (1 byte)
            // 412-N    data (N bytes)

            byte[] content = File.ReadAllBytes(OldFirmwareFile.Text);

            if (content.Length > 411)
            {
                // verify signature
                if (content[0] == 0x13 && content[1] == 0x22 && content[2] == 0x31)
                {
                    DeviceId.Text = content[3].ToString();
                    DeviceCode.Text = content[4].ToString();

                    byte[] version = new byte[4];
                    Array.Copy(content, 5, version, 0, version.Length);

                    // allowed firmware versions
                    AllowedFirmwareVersions.Text = version[0].ToString() + "," + version[1].ToString() + "," + version[2].ToString() + "," + version[3].ToString();

                    List<byte[]> allowed = new List<byte[]>();
                    for (int x = 0; x < 400; x += 4)
                    {
                        if (content[9 + x] != 0 || content[10 + x] != 0 || content[11 + x] != 0 || content[12 + x] != 0)
                        {
                            if (AllowedFirmwareVersions.Text.Trim() != "") AllowedFirmwareVersions.Text += "; ";
                            AllowedFirmwareVersions.Text += content[9 + x].ToString() + "," + content[10 + x].ToString() + "," + content[11 + x].ToString() + "," + content[12 + x].ToString();
                        }
                    }

                    int pageSize = (content[409] << 8) + content[410];
                    if (pageSize == 512)
                        DevicePageSize.SelectedIndex = 0;
                    else 
                        DevicePageSize.SelectedIndex = 1;
                }
                else
                    MessageBox.Show("Selected file doesn't seem to be encrypted .bin file");
            }
            else
                MessageBox.Show("Selected file is corrupted.");
        }
    }
}
