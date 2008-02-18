using System;

namespace DeveloperTerminal 
{
	public class XMLData
    {
        #region COM port parameters

        public string COMPort = "Not Initialized";
		public int Baud = 0;
		public int Databits = 0;
        public string Parity = "Not Initialized";
        public string Stopbits = "Not Initialized";
        public string Handshake = "Not Initialized";

        #endregion

        #region Terminal Characters
        public string TerminalCharacters = "";
        #endregion

        #region Initialize Macros

        // Initialize the Macro text
		public string MacroText1 = " ";
		public string MacroText2 = " ";
		public string MacroText3 = " ";
		public string MacroText4 = " ";
		public string MacroText5 = " ";
		public string MacroText6 = " ";
		public string MacroText7 = " ";
		public string MacroText8 = " ";
		public string MacroText9 = " ";
		public string MacroText10 = " ";
		public string MacroText11 = " ";
		public string MacroText12 = " ";
		public string MacroText13 = " ";
		public string MacroText14 = " ";
		public string MacroText15 = " ";
		public string MacroText16 = " ";
		public string MacroText17 = " ";
		public string MacroText18 = " ";
		public string MacroText19 = " ";
		public string MacroText20 = " ";
		public string MacroText21 = " ";
		public string MacroText22 = " ";
		public string MacroText23 = " ";
		public string MacroText24 = " ";
		public string MacroText25 = " ";
		public string MacroText26 = " ";
		public string MacroText27 = " ";
		public string MacroText28 = " ";
		public string MacroText29 = " ";
		public string MacroText30 = " ";
		public string MacroText31 = " ";
		public string MacroText32 = " ";
		public string MacroText33 = " ";
		public string MacroText34 = " ";
		public string MacroText35 = " ";
		public string MacroText36 = " ";
		public string MacroText37 = " ";
		public string MacroText38 = " ";
		public string MacroText39 = " ";
		public string MacroText40 = " ";
		public string MacroText41 = " ";
		public string MacroText42 = " ";
		public string MacroText43 = " ";
		public string MacroText44 = " ";
		public string MacroText45 = " ";
		public string MacroText46 = " ";
		public string MacroText47 = " ";
		public string MacroText48 = " ";
		public string MacroText49 = " ";
		public string MacroText50 = " ";
		public string MacroText51 = " ";
		public string MacroText52 = " ";
		
		// Initialize the Macro Data
		public string MacroData1 = " ";
		public string MacroData2 = " ";
		public string MacroData3 = " ";
		public string MacroData4 = " ";
		public string MacroData5 = " ";
		public string MacroData6 = " ";
		public string MacroData7 = " ";
		public string MacroData8 = " ";

		public string MacroData9 = " ";
		public string MacroData10 = " ";
		public string MacroData11 = " ";
		public string MacroData12 = " ";
		public string MacroData13 = " ";
		public string MacroData14 = " ";
		public string MacroData15 = " ";
		public string MacroData16 = " ";
		public string MacroData17 = " ";
		public string MacroData18 = " ";
		public string MacroData19 = " ";
		public string MacroData20 = " ";
		public string MacroData21 = " ";
		public string MacroData22 = " ";
		public string MacroData23 = " ";
		public string MacroData24 = " ";
		public string MacroData25 = " ";
		public string MacroData26 = " ";
		public string MacroData27 = " ";
		public string MacroData28 = " ";
		public string MacroData29 = " ";
		public string MacroData30 = " ";
		public string MacroData31 = " ";
		public string MacroData32 = " ";
		public string MacroData33 = " ";
		public string MacroData34 = " ";
		public string MacroData35 = " ";
		public string MacroData36 = " ";
		public string MacroData37 = " ";
		public string MacroData38 = " ";
		public string MacroData39 = " ";
		public string MacroData40 = " ";
		public string MacroData41 = " ";
		public string MacroData42 = " ";
		public string MacroData43 = " ";
		public string MacroData44 = " ";
		public string MacroData45 = " ";
		public string MacroData46 = " ";
		public string MacroData47 = " ";
		public string MacroData48 = " ";
		public string MacroData49 = " ";
		public string MacroData50 = " ";
		public string MacroData51 = " ";
		public string MacroData52 = " ";
		
		// Initialize the Macro Type
		public string MacroType1 = " ";
		public string MacroType2 = " ";
		public string MacroType3 = " ";
		public string MacroType4 = " ";
		public string MacroType5 = " ";
		public string MacroType6 = " ";
		public string MacroType7 = " ";
		public string MacroType8 = " ";
		public string MacroType9 = " ";
		public string MacroType10 = " ";
		public string MacroType11 = " ";
		public string MacroType12 = " ";
		public string MacroType13 = " ";
		public string MacroType14 = " ";
		public string MacroType15 = " ";
		public string MacroType16 = " ";
		public string MacroType17 = " ";
		public string MacroType18 = " ";
		public string MacroType19 = " ";
		public string MacroType20 = " ";
		public string MacroType21 = " ";
		public string MacroType22 = " ";
		public string MacroType23 = " ";
		public string MacroType24 = " ";
		public string MacroType25 = " ";
		public string MacroType26 = " ";
		public string MacroType27 = " ";
		public string MacroType28 = " ";
		public string MacroType29 = " ";
		public string MacroType30 = " ";
		public string MacroType31 = " ";	
		public string MacroType32 = " ";
		public string MacroType33 = " ";
		public string MacroType34 = " ";
		public string MacroType35 = " ";
		public string MacroType36 = " ";
		public string MacroType37 = " ";
		public string MacroType38 = " ";
		public string MacroType39 = " ";
		public string MacroType40 = " ";
		public string MacroType41 = " ";
		public string MacroType42 = " ";
		public string MacroType43 = " ";
		public string MacroType44 = " ";
		public string MacroType45 = " ";
		public string MacroType46 = " ";
		public string MacroType47 = " ";
		public string MacroType48 = " ";
		public string MacroType49 = " ";
		public string MacroType50 = " ";
		public string MacroType51 = " ";	
		public string MacroType52 = " ";

        #endregion
    }
}
