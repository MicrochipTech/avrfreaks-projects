<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.0.1">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting keepoldvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="9" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="3_My_prog">
<packages>
<package name="USB-B">
<pad name="5" x="-4.8" y="-4.71" drill="2.5" shape="square"/>
<pad name="6" x="7.24" y="-4.71" drill="2.5" shape="square"/>
<wire x1="-4.75" y1="-14.95" x2="-4.75" y2="2.04" width="0.127" layer="21"/>
<wire x1="-4.75" y1="2.04" x2="7.29" y2="2.04" width="0.127" layer="21"/>
<wire x1="7.29" y1="2.04" x2="7.29" y2="-14.95" width="0.127" layer="21"/>
<wire x1="7.29" y1="-14.95" x2="-4.75" y2="-14.95" width="0.127" layer="21"/>
<pad name="3" x="-0.03" y="-2" drill="1" shape="square"/>
<pad name="4" x="2.47" y="-2" drill="1" shape="square"/>
<pad name="2" x="-0.01" y="0.015" drill="1" shape="square"/>
<pad name="1" x="2.475" y="0.02" drill="1" shape="square"/>
</package>
</packages>
<symbols>
<symbol name="USB-B">
<pin name="+VE" x="-2.54" y="7.62" length="short"/>
<pin name="D-VE" x="-2.54" y="5.08" length="short"/>
<pin name="D+VE" x="-2.54" y="2.54" length="short"/>
<pin name="GND" x="-2.54" y="0" length="short"/>
<pin name="GND@1" x="-2.54" y="-2.54" length="short"/>
<pin name="GND@2" x="-2.54" y="-5.08" length="short"/>
<wire x1="0" y1="10.16" x2="0" y2="-7.62" width="0.254" layer="94"/>
<wire x1="0" y1="-7.62" x2="12.7" y2="-7.62" width="0.254" layer="94"/>
<wire x1="12.7" y1="-7.62" x2="12.7" y2="10.16" width="0.254" layer="94"/>
<wire x1="12.7" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<text x="1.27" y="11.43" size="1.27" layer="94">&gt;NAME</text>
<text x="2.54" y="-10.16" size="1.27" layer="94">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="USB-B" prefix="SKT">
<description>RS 515-1967</description>
<gates>
<gate name="G$1" symbol="USB-B" x="-12.7" y="15.24"/>
</gates>
<devices>
<device name="" package="USB-B">
<connects>
<connect gate="G$1" pin="+VE" pad="1"/>
<connect gate="G$1" pin="D+VE" pad="3"/>
<connect gate="G$1" pin="D-VE" pad="2"/>
<connect gate="G$1" pin="GND" pad="4"/>
<connect gate="G$1" pin="GND@1" pad="5"/>
<connect gate="G$1" pin="GND@2" pad="6"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="1_Home_library">
<packages>
<package name="1206">
<wire x1="1.6" y1="-0.8" x2="1.6" y2="0.8" width="0.127" layer="21"/>
<wire x1="1.6" y1="0.8" x2="-1.6" y2="0.8" width="0.127" layer="21"/>
<wire x1="-1.6" y1="0.8" x2="-1.6" y2="-0.8" width="0.127" layer="21"/>
<wire x1="-1.6" y1="-0.8" x2="1.6" y2="-0.8" width="0.127" layer="21"/>
<text x="-3.1" y="1.55" size="1.27" layer="27">&gt;VALUE</text>
<smd name="P$1" x="-1.75" y="0" dx="2.2" dy="2" layer="1" rot="R180"/>
<smd name="P$2" x="1.7" y="0" dx="2.2" dy="2" layer="1" rot="R180"/>
<text x="-2.286" y="0.508" size="1.27" layer="21">+</text>
<text x="-2.54" y="-2.54" size="1.27" layer="25">&gt;NAME</text>
</package>
<package name="SM_NO_PUSH_BUTTON">
<description>RE 51-0709</description>
<wire x1="0" y1="0" x2="0" y2="3" width="0.127" layer="21"/>
<wire x1="0" y1="3" x2="6" y2="3" width="0.127" layer="21"/>
<wire x1="6" y1="3" x2="6" y2="0" width="0.127" layer="21"/>
<wire x1="6" y1="0" x2="0" y2="0" width="0.127" layer="21"/>
<smd name="P$1" x="-2" y="1.5" dx="3.3" dy="2" layer="1"/>
<smd name="P$2" x="8" y="1.5" dx="3.5" dy="2" layer="1"/>
<text x="0" y="3.5" size="1.27" layer="25">&gt;NAME</text>
<text x="0" y="-1.5" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="DIL28-3">
<description>&lt;B&gt;Dual In Line&lt;/B&gt; 0.3 inch</description>
<wire x1="-18.542" y1="-0.635" x2="-18.542" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="0.635" x2="-18.542" y2="-0.635" width="0.1524" layer="21" curve="-180"/>
<wire x1="-18.542" y1="-2.794" x2="18.542" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="2.794" x2="-18.542" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="2.794" x2="18.542" y2="2.794" width="0.1524" layer="21"/>
<wire x1="18.542" y1="2.794" x2="18.542" y2="-2.794" width="0.1524" layer="21"/>
<pad name="1" x="-16.51" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-13.97" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-11.43" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-8.89" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-6.35" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="-1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="6.35" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="8.89" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="11.43" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="13.97" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="16.51" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="16.51" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="13.97" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="11.43" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="8.89" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="6.35" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="21" x="1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="22" x="-1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="23" x="-3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="24" x="-6.35" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="25" x="-8.89" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="26" x="-11.43" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="27" x="-13.97" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="28" x="-16.51" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-19.2024" y="-2.54" size="1.778" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-15.875" y="-0.635" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="CAP">
<rectangle x1="-2.54" y1="-1.905" x2="-1.27" y2="1.905" layer="94"/>
<rectangle x1="1.27" y1="-1.905" x2="2.54" y2="1.905" layer="94"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short"/>
<pin name="2" x="5.08" y="0" visible="off" length="short" rot="R180"/>
<text x="-1.27" y="-3.81" size="1.27" layer="96">&gt;VALUE</text>
<text x="-3.556" y="0.254" size="1.27" layer="94">+</text>
<text x="-2.54" y="2.54" size="1.778" layer="95">&gt;NAME</text>
</symbol>
<symbol name="RES">
<wire x1="-2.54" y1="0.762" x2="2.54" y2="0.762" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.762" x2="2.54" y2="-0.762" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.762" x2="-2.54" y2="-0.762" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.762" x2="-2.54" y2="0.762" width="0.254" layer="94"/>
<text x="3.302" y="0.508" size="1.778" layer="94">&gt;VALUE</text>
<text x="3.302" y="-2.286" size="1.778" layer="94">&gt;NAME</text>
<pin name="P$1" x="-5.08" y="0" visible="off" length="short"/>
<pin name="P$2" x="5.08" y="0" visible="off" length="short" rot="R180"/>
</symbol>
<symbol name="FERRITE_CHOKE">
<circle x="-0.635" y="1.27" radius="1.27" width="0.254" layer="94"/>
<circle x="0.635" y="1.27" radius="1.27" width="0.254" layer="94"/>
<wire x1="-5.08" y1="0" x2="5.08" y2="0" width="0.254" layer="94"/>
<pin name="1" x="-5.08" y="0" visible="off" length="point"/>
<pin name="2" x="5.08" y="0" visible="off" length="point" rot="R180"/>
<circle x="-5.08" y="0" radius="0.4490125" width="0.254" layer="94"/>
<circle x="5.08" y="0" radius="0.4490125" width="0.254" layer="94"/>
</symbol>
<symbol name="SM_NO_PUSH_BUTTON">
<description>RE 51-0709</description>
<pin name="1" x="-2.54" y="0" visible="off" length="short"/>
<pin name="2" x="10.16" y="0" visible="off" length="short" rot="R180"/>
<circle x="0.635" y="0" radius="0.898025" width="0.254" layer="94"/>
<circle x="7.62" y="0" radius="0.898025" width="0.254" layer="94"/>
<wire x1="0.635" y1="0" x2="7.62" y2="1.27" width="0.254" layer="94"/>
</symbol>
<symbol name="23-I/O-2">
<wire x1="-17.78" y1="30.48" x2="20.32" y2="30.48" width="0.254" layer="94"/>
<wire x1="20.32" y1="30.48" x2="20.32" y2="-33.02" width="0.254" layer="94"/>
<wire x1="20.32" y1="-33.02" x2="-17.78" y2="-33.02" width="0.254" layer="94"/>
<wire x1="-17.78" y1="-33.02" x2="-17.78" y2="30.48" width="0.254" layer="94"/>
<text x="-17.78" y="-35.56" size="1.778" layer="95">&gt;NAME</text>
<text x="-17.78" y="31.75" size="1.778" layer="96">&gt;VALUE</text>
<pin name="PB5(SCK)" x="25.4" y="-30.48" length="middle" rot="R180"/>
<pin name="PB7(XTAL2/TOSC2)" x="-22.86" y="5.08" length="middle"/>
<pin name="PB6(XTAL1/TOSC1)" x="-22.86" y="10.16" length="middle"/>
<pin name="GND@1" x="-22.86" y="-2.54" length="middle" direction="pwr"/>
<pin name="VCC@1" x="-22.86" y="-7.62" length="middle" direction="pwr"/>
<pin name="GND" x="-22.86" y="22.86" length="middle" direction="pwr"/>
<pin name="AREF" x="-22.86" y="20.32" length="middle" direction="pas"/>
<pin name="AVCC" x="-22.86" y="17.78" length="middle" direction="pwr"/>
<pin name="PB4(MISO)" x="25.4" y="-27.94" length="middle" rot="R180"/>
<pin name="PB3(MOSI/OC2)" x="25.4" y="-25.4" length="middle" rot="R180"/>
<pin name="PB2(SS/OC1B)" x="25.4" y="-22.86" length="middle" rot="R180"/>
<pin name="PB1(OC1A)" x="25.4" y="-20.32" length="middle" rot="R180"/>
<pin name="PB0(ICP)" x="25.4" y="-17.78" length="middle" rot="R180"/>
<pin name="PD7(AIN1)" x="25.4" y="-12.7" length="middle" rot="R180"/>
<pin name="PD6(AIN0)" x="25.4" y="-10.16" length="middle" rot="R180"/>
<pin name="PD5(T1)" x="25.4" y="-7.62" length="middle" rot="R180"/>
<pin name="PD4(XCK/T0)" x="25.4" y="-5.08" length="middle" rot="R180"/>
<pin name="PD3(INT1)" x="25.4" y="-2.54" length="middle" rot="R180"/>
<pin name="PD2(INT0)" x="25.4" y="0" length="middle" rot="R180"/>
<pin name="PD1(TXD)" x="25.4" y="2.54" length="middle" rot="R180"/>
<pin name="PD0(RXD)" x="25.4" y="5.08" length="middle" rot="R180"/>
<pin name="PC5(ADC5/SCL)" x="25.4" y="15.24" length="middle" rot="R180"/>
<pin name="PC4(ADC4/SDA)" x="25.4" y="17.78" length="middle" rot="R180"/>
<pin name="PC3(ADC3)" x="25.4" y="20.32" length="middle" rot="R180"/>
<pin name="PC2(ADC2)" x="25.4" y="22.86" length="middle" rot="R180"/>
<pin name="PC1(ADC1)" x="25.4" y="25.4" length="middle" rot="R180"/>
<pin name="PC0(ADC0)" x="25.4" y="27.94" length="middle" rot="R180"/>
<pin name="PC6(/RESET)" x="-22.86" y="27.94" length="middle" function="dot"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="1206C" prefix="C" uservalue="yes">
<description>4.7uF RS 538-2335   100nF  RS 264-4179</description>
<gates>
<gate name="G$1" symbol="CAP" x="0" y="0"/>
</gates>
<devices>
<device name="" package="1206">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="1206R" prefix="R" uservalue="yes">
<gates>
<gate name="G$1" symbol="RES" x="-2.54" y="-2.54"/>
</gates>
<devices>
<device name="" package="1206">
<connects>
<connect gate="G$1" pin="P$1" pad="P$1"/>
<connect gate="G$1" pin="P$2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CHOKE" prefix="L">
<description>RS 724-1582 1206 inductor</description>
<gates>
<gate name="G$1" symbol="FERRITE_CHOKE" x="-22.86" y="12.7"/>
</gates>
<devices>
<device name="" package="1206">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="NO_SW" prefix="SW">
<description>SM NO push button RE 51-0709</description>
<gates>
<gate name="G$1" symbol="SM_NO_PUSH_BUTTON" x="-35.56" y="22.86"/>
</gates>
<devices>
<device name="" package="SM_NO_PUSH_BUTTON">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MEGA8-P" prefix="IC">
<description>&lt;b&gt;MICROCONTROLLER&lt;/b&gt;&lt;p&gt;
8 Kbytes FLASH, 1 kbytes SRAM, 512 bytes EEPROM, USART, 4-channel 10 bit ADC, 2-channel 8 bit ADC&lt;br&gt;
Pin compatible with Atmega48, ATMega88, ATMega168&lt;br&gt;
Source: avr.lbr</description>
<gates>
<gate name="G$1" symbol="23-I/O-2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DIL28-3">
<connects>
<connect gate="G$1" pin="AREF" pad="21"/>
<connect gate="G$1" pin="AVCC" pad="20"/>
<connect gate="G$1" pin="GND" pad="22"/>
<connect gate="G$1" pin="GND@1" pad="8"/>
<connect gate="G$1" pin="PB0(ICP)" pad="14"/>
<connect gate="G$1" pin="PB1(OC1A)" pad="15"/>
<connect gate="G$1" pin="PB2(SS/OC1B)" pad="16"/>
<connect gate="G$1" pin="PB3(MOSI/OC2)" pad="17"/>
<connect gate="G$1" pin="PB4(MISO)" pad="18"/>
<connect gate="G$1" pin="PB5(SCK)" pad="19"/>
<connect gate="G$1" pin="PB6(XTAL1/TOSC1)" pad="9"/>
<connect gate="G$1" pin="PB7(XTAL2/TOSC2)" pad="10"/>
<connect gate="G$1" pin="PC0(ADC0)" pad="23"/>
<connect gate="G$1" pin="PC1(ADC1)" pad="24"/>
<connect gate="G$1" pin="PC2(ADC2)" pad="25"/>
<connect gate="G$1" pin="PC3(ADC3)" pad="26"/>
<connect gate="G$1" pin="PC4(ADC4/SDA)" pad="27"/>
<connect gate="G$1" pin="PC5(ADC5/SCL)" pad="28"/>
<connect gate="G$1" pin="PC6(/RESET)" pad="1"/>
<connect gate="G$1" pin="PD0(RXD)" pad="2"/>
<connect gate="G$1" pin="PD1(TXD)" pad="3"/>
<connect gate="G$1" pin="PD2(INT0)" pad="4"/>
<connect gate="G$1" pin="PD3(INT1)" pad="5"/>
<connect gate="G$1" pin="PD4(XCK/T0)" pad="6"/>
<connect gate="G$1" pin="PD5(T1)" pad="11"/>
<connect gate="G$1" pin="PD6(AIN0)" pad="12"/>
<connect gate="G$1" pin="PD7(AIN1)" pad="13"/>
<connect gate="G$1" pin="VCC@1" pad="7"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="supply1">
<description>&lt;b&gt;Supply Symbols&lt;/b&gt;&lt;p&gt;
 GND, VCC, 0V, +5V, -5V, etc.&lt;p&gt;
 Please keep in mind, that these devices are necessary for the
 automatic wiring of the supply signals.&lt;p&gt;
 The pin name defined in the symbol is identical to the net which is to be wired automatically.&lt;p&gt;
 In this library the device names are the same as the pin names of the symbols, therefore the correct signal names appear next to the supply symbols in the schematic.&lt;p&gt;
 &lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="+5V">
<wire x1="1.27" y1="-1.905" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="-1.27" y2="-1.905" width="0.254" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="+5V" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="0V">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-1.905" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="0V" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="+5V" prefix="P+">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="+5V" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="0V" prefix="GND">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="0V" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="1_marko">
<packages>
<package name="4_DIGIT_7SEGMENT_DISPLAY">
<pad name="P$1" x="0" y="0" drill="0.8" shape="square"/>
<pad name="P$2" x="2.54" y="0" drill="0.8" shape="square"/>
<pad name="P$3" x="5.08" y="0" drill="0.8" shape="square"/>
<pad name="P$4" x="7.62" y="0" drill="0.8" shape="square"/>
<pad name="P$5" x="10.16" y="0" drill="0.8" shape="square"/>
<pad name="P$6" x="12.7" y="0" drill="0.8" shape="square"/>
<pad name="P$7" x="12.7" y="15.24" drill="0.8" shape="square"/>
<pad name="P$8" x="10.16" y="15.24" drill="0.8" shape="square"/>
<pad name="P$9" x="7.62" y="15.24" drill="0.8" shape="square"/>
<pad name="P$10" x="5.08" y="15.24" drill="0.8" shape="square"/>
<pad name="P$11" x="2.54" y="15.24" drill="0.8" shape="square"/>
<pad name="P$12" x="0" y="15.24" drill="0.8" shape="square"/>
<wire x1="-19.05" y1="17.145" x2="31.75" y2="17.145" width="0.127" layer="21"/>
<wire x1="31.75" y1="17.145" x2="31.75" y2="-1.905" width="0.127" layer="21"/>
<wire x1="31.75" y1="-1.905" x2="-19.05" y2="-1.905" width="0.127" layer="21"/>
<wire x1="-19.05" y1="-1.905" x2="-19.05" y2="17.145" width="0.127" layer="21"/>
<text x="-10.16" y="15.24" size="1.27" layer="21">&gt;NAME</text>
<text x="15.24" y="0" size="1.27" layer="21">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="4_DIGIT_7SEGMENT_DISPLAY">
<pin name="E" x="-2.54" y="-12.7" visible="pin" length="short"/>
<pin name="D" x="-2.54" y="-10.16" visible="pin" length="short"/>
<pin name="CLON" x="-2.54" y="-7.62" visible="pin" length="short"/>
<pin name="C" x="-2.54" y="-5.08" visible="pin" length="short"/>
<pin name="G" x="-2.54" y="-2.54" visible="pin" length="short"/>
<pin name="DIG4" x="-2.54" y="0" visible="pin" length="short"/>
<pin name="B" x="-2.54" y="2.54" visible="pin" length="short"/>
<pin name="DIG3" x="-2.54" y="5.08" visible="pin" length="short"/>
<pin name="DIG2" x="-2.54" y="7.62" visible="pin" length="short"/>
<pin name="F" x="-2.54" y="10.16" visible="pin" length="short"/>
<pin name="A" x="-2.54" y="12.7" visible="pin" length="short"/>
<pin name="DIG1" x="-2.54" y="15.24" visible="pin" length="short"/>
<wire x1="0" y1="17.78" x2="0" y2="-15.24" width="0.254" layer="94"/>
<wire x1="0" y1="-15.24" x2="7.62" y2="-15.24" width="0.254" layer="94"/>
<wire x1="7.62" y1="-15.24" x2="7.62" y2="17.78" width="0.254" layer="94"/>
<wire x1="7.62" y1="17.78" x2="0" y2="17.78" width="0.254" layer="94"/>
<text x="8.89" y="12.7" size="1.778" layer="94" rot="R270">&gt;NAME</text>
<text x="10.16" y="-12.7" size="1.778" layer="94" rot="R90">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="4_DIGIT_7SEGMENT_DISPLAY" prefix="7_SEG">
<description>Common Anode RE 55-1674</description>
<gates>
<gate name="G$1" symbol="4_DIGIT_7SEGMENT_DISPLAY" x="20.32" y="5.08"/>
</gates>
<devices>
<device name="" package="4_DIGIT_7SEGMENT_DISPLAY">
<connects>
<connect gate="G$1" pin="A" pad="P$11"/>
<connect gate="G$1" pin="B" pad="P$7"/>
<connect gate="G$1" pin="C" pad="P$4"/>
<connect gate="G$1" pin="CLON" pad="P$3"/>
<connect gate="G$1" pin="D" pad="P$2"/>
<connect gate="G$1" pin="DIG1" pad="P$12"/>
<connect gate="G$1" pin="DIG2" pad="P$9"/>
<connect gate="G$1" pin="DIG3" pad="P$8"/>
<connect gate="G$1" pin="DIG4" pad="P$6"/>
<connect gate="G$1" pin="E" pad="P$1"/>
<connect gate="G$1" pin="F" pad="P$10"/>
<connect gate="G$1" pin="G" pad="P$5"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="1_3V3_Bootloader">
<packages>
<package name="SSOP16">
<description>&lt;b&gt;SSOP-16&lt;/b&gt;&lt;p&gt;
Auto generated by &lt;i&gt;make-symbol-device-package-bsdl.ulp Rev. 43&lt;/i&gt;&lt;br&gt;
Source: &lt;a href="http://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT230X.pdf"&gt; Data sheet &lt;/a&gt;</description>
<smd name="1" x="-3.0178" y="2.2225" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="2.0701" x2="-1.9939" y2="2.3749" layer="51"/>
<smd name="2" x="-3.0178" y="1.5875" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="1.4351" x2="-1.9939" y2="1.7399" layer="51"/>
<smd name="3" x="-3.0178" y="0.9525" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="0.8001" x2="-1.9939" y2="1.1049" layer="51"/>
<smd name="4" x="-3.0178" y="0.3175" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="0.1651" x2="-1.9939" y2="0.4699" layer="51"/>
<smd name="5" x="-3.0178" y="-0.3175" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="-0.4699" x2="-1.9939" y2="-0.1651" layer="51"/>
<smd name="6" x="-3.0178" y="-0.9525" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="-1.1049" x2="-1.9939" y2="-0.8001" layer="51"/>
<smd name="7" x="-3.0178" y="-1.5875" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="-1.7399" x2="-1.9939" y2="-1.4351" layer="51"/>
<smd name="8" x="-3.0178" y="-2.2225" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="-3.0988" y1="-2.3749" x2="-1.9939" y2="-2.0701" layer="51"/>
<smd name="9" x="3.0178" y="-2.2225" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="-2.3749" x2="3.0988" y2="-2.0701" layer="51"/>
<smd name="10" x="3.0178" y="-1.5875" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="-1.7399" x2="3.0988" y2="-1.4351" layer="51"/>
<smd name="11" x="3.0178" y="-0.9525" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="-1.1049" x2="3.0988" y2="-0.8001" layer="51"/>
<smd name="12" x="3.0178" y="-0.3175" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="-0.4699" x2="3.0988" y2="-0.1651" layer="51"/>
<smd name="13" x="3.0178" y="0.3175" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="0.1651" x2="3.0988" y2="0.4699" layer="51"/>
<smd name="14" x="3.0178" y="0.9525" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="0.8001" x2="3.0988" y2="1.1049" layer="51"/>
<smd name="15" x="3.0178" y="1.5875" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="1.4351" x2="3.0988" y2="1.7399" layer="51"/>
<smd name="16" x="3.0178" y="2.2225" dx="1.5" dy="0.4064" layer="1"/>
<rectangle x1="1.9939" y1="2.0701" x2="3.0988" y2="2.3749" layer="51"/>
<wire x1="-0.508" y1="2.49931875" x2="0.508" y2="2.49931875" width="0.2032" layer="21" curve="180"/>
<wire x1="-1.99131875" y1="-2.49931875" x2="1.99131875" y2="-2.49931875" width="0.2032" layer="21"/>
<wire x1="1.99131875" y1="-2.49931875" x2="1.99131875" y2="2.49931875" width="0.2032" layer="21"/>
<wire x1="1.99131875" y1="2.49931875" x2="-1.99131875" y2="2.49931875" width="0.2032" layer="21"/>
<wire x1="-1.99131875" y1="2.49931875" x2="-1.99131875" y2="-2.49931875" width="0.2032" layer="21"/>
<text x="-3.175" y="2.794" size="1.778" layer="25">&gt;NAME</text>
<text x="-3.429" y="-4.826" size="1.778" layer="27">&gt;VALUE</text>
</package>
<package name="QFN16">
<description>&lt;b&gt;QFN-16 4 x 4 mm&lt;/b&gt;&lt;p&gt;
Source: &lt;a href="http://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT230X.pdf"&gt; Data sheet &lt;/a&gt;</description>
<wire x1="-1.9" y1="1.9" x2="-1.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="-1.9" y1="-1.9" x2="1.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="1.9" y1="-1.9" x2="1.9" y2="1.9" width="0.2032" layer="21"/>
<wire x1="1.9" y1="1.9" x2="-1.9" y2="1.9" width="0.2032" layer="21"/>
<smd name="TAB" x="0" y="0" dx="1" dy="1" layer="1" stop="no"/>
<smd name="1" x="-1.725" y="0.975" dx="0.7" dy="0.35" layer="1" stop="no"/>
<smd name="2" x="-1.725" y="0.325" dx="0.7" dy="0.35" layer="1" stop="no"/>
<smd name="3" x="-1.725" y="-0.325" dx="0.7" dy="0.35" layer="1" stop="no"/>
<smd name="4" x="-1.725" y="-0.975" dx="0.7" dy="0.35" layer="1" stop="no"/>
<smd name="5" x="-0.975" y="-1.725" dx="0.7" dy="0.35" layer="1" rot="R90" stop="no"/>
<smd name="6" x="-0.325" y="-1.725" dx="0.7" dy="0.35" layer="1" rot="R90" stop="no"/>
<smd name="7" x="0.325" y="-1.725" dx="0.7" dy="0.35" layer="1" rot="R90" stop="no"/>
<smd name="8" x="0.975" y="-1.725" dx="0.7" dy="0.35" layer="1" rot="R90" stop="no"/>
<smd name="9" x="1.725" y="-0.975" dx="0.7" dy="0.35" layer="1" rot="R180" stop="no"/>
<smd name="10" x="1.725" y="-0.325" dx="0.7" dy="0.35" layer="1" rot="R180" stop="no"/>
<smd name="11" x="1.725" y="0.325" dx="0.7" dy="0.35" layer="1" rot="R180" stop="no"/>
<smd name="12" x="1.725" y="0.975" dx="0.7" dy="0.35" layer="1" rot="R180" stop="no"/>
<smd name="13" x="0.975" y="1.725" dx="0.7" dy="0.35" layer="1" rot="R270" stop="no"/>
<smd name="14" x="0.325" y="1.725" dx="0.7" dy="0.35" layer="1" rot="R270" stop="no"/>
<smd name="15" x="-0.325" y="1.725" dx="0.7" dy="0.35" layer="1" rot="R270" stop="no"/>
<smd name="16" x="-0.975" y="1.725" dx="0.7" dy="0.35" layer="1" rot="R270" stop="no"/>
<polygon width="0.2032" layer="1">
<vertex x="-0.9525" y="-0.9525"/>
<vertex x="-0.9525" y="0.635"/>
<vertex x="-0.635" y="0.9525"/>
<vertex x="0.9525" y="0.9525"/>
<vertex x="0.9525" y="-0.9525"/>
</polygon>
<polygon width="0.254" layer="29">
<vertex x="-0.9525" y="-0.9525"/>
<vertex x="-0.9525" y="0.635"/>
<vertex x="-0.635" y="0.9525"/>
<vertex x="0.9525" y="0.9525"/>
<vertex x="0.9525" y="-0.9525"/>
</polygon>
<polygon width="0.15" layer="31">
<vertex x="-0.9525" y="-0.9525"/>
<vertex x="-0.9525" y="0.635"/>
<vertex x="-0.635" y="0.9525"/>
<vertex x="0.9525" y="0.9525"/>
<vertex x="0.9525" y="-0.9525"/>
</polygon>
<rectangle x1="-2.15" y1="0.775" x2="-1.3" y2="1.175" layer="29"/>
<rectangle x1="-2.15" y1="0.125" x2="-1.3" y2="0.525" layer="29"/>
<rectangle x1="-2.15" y1="-0.525" x2="-1.3" y2="-0.125" layer="29"/>
<rectangle x1="-2.15" y1="-1.175" x2="-1.3" y2="-0.775" layer="29"/>
<rectangle x1="-1.4" y1="-1.925" x2="-0.55" y2="-1.525" layer="29" rot="R90"/>
<rectangle x1="-0.75" y1="-1.925" x2="0.1" y2="-1.525" layer="29" rot="R90"/>
<rectangle x1="-0.1" y1="-1.925" x2="0.75" y2="-1.525" layer="29" rot="R90"/>
<rectangle x1="0.55" y1="-1.925" x2="1.4" y2="-1.525" layer="29" rot="R90"/>
<rectangle x1="1.3" y1="-1.175" x2="2.15" y2="-0.775" layer="29" rot="R180"/>
<rectangle x1="1.3" y1="-0.525" x2="2.15" y2="-0.125" layer="29" rot="R180"/>
<rectangle x1="1.3" y1="0.125" x2="2.15" y2="0.525" layer="29" rot="R180"/>
<rectangle x1="1.3" y1="0.775" x2="2.15" y2="1.175" layer="29" rot="R180"/>
<rectangle x1="0.55" y1="1.525" x2="1.4" y2="1.925" layer="29" rot="R270"/>
<rectangle x1="-0.1" y1="1.525" x2="0.75" y2="1.925" layer="29" rot="R270"/>
<rectangle x1="-0.75" y1="1.525" x2="0.1" y2="1.925" layer="29" rot="R270"/>
<rectangle x1="-1.4" y1="1.525" x2="-0.55" y2="1.925" layer="29" rot="R270"/>
<text x="-2.15" y="2.277" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.15" y="-4.055" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="SOT23">
<wire x1="0" y1="0" x2="3" y2="0" width="0.127" layer="21"/>
<wire x1="3" y1="0" x2="3" y2="-1.4" width="0.127" layer="21"/>
<wire x1="3" y1="-1.4" x2="0" y2="-1.4" width="0.127" layer="21"/>
<wire x1="0" y1="-1.4" x2="0" y2="0" width="0.127" layer="21"/>
<smd name="1" x="0.56" y="-2.08" dx="0.85" dy="1.75" layer="1"/>
<smd name="3" x="1.5" y="0.57" dx="0.75" dy="1.75" layer="1"/>
<smd name="2" x="2.47" y="-2.07" dx="0.85" dy="1.75" layer="1"/>
</package>
<package name="2SK3018">
<wire x1="-0.95" y1="0.65" x2="1" y2="0.65" width="0.127" layer="21"/>
<wire x1="1" y1="0.65" x2="1" y2="-0.6" width="0.127" layer="21"/>
<wire x1="1" y1="-0.6" x2="-1" y2="-0.6" width="0.127" layer="21"/>
<wire x1="-1" y1="-0.6" x2="-1" y2="0.65" width="0.127" layer="21"/>
<smd name="3" x="0" y="0.9" dx="1" dy="0.5" layer="1" rot="R90"/>
<smd name="2" x="-0.5" y="-0.8" dx="1" dy="0.5" layer="1" rot="R90"/>
<smd name="1" x="0.5" y="-0.8" dx="1" dy="0.5" layer="1" rot="R90"/>
</package>
</packages>
<symbols>
<symbol name="FT230X">
<pin name="TXD" x="-15.24" y="7.62" length="short" direction="out"/>
<pin name="!RTS" x="-15.24" y="5.08" length="short" direction="out"/>
<pin name="VCCIO" x="-15.24" y="2.54" length="short" direction="pwr"/>
<pin name="RXD" x="-15.24" y="0" length="short" direction="in"/>
<pin name="GND" x="-15.24" y="-2.54" length="short" direction="pwr"/>
<pin name="!CTS" x="-15.24" y="-5.08" length="short" direction="in"/>
<pin name="CBUS2" x="-15.24" y="-7.62" length="short"/>
<pin name="USBDP" x="-15.24" y="-10.16" length="short" direction="in"/>
<pin name="USBDM" x="15.24" y="-10.16" length="short" direction="in" rot="R180"/>
<pin name="3V3OUT" x="15.24" y="-7.62" length="short" direction="out" rot="R180"/>
<pin name="!RESET" x="15.24" y="-5.08" length="short" direction="in" rot="R180"/>
<pin name="VCC" x="15.24" y="-2.54" length="short" direction="pwr" rot="R180"/>
<pin name="GND@1" x="15.24" y="0" length="short" direction="pwr" rot="R180"/>
<pin name="CBUS1" x="15.24" y="2.54" length="short" rot="R180"/>
<pin name="CBUS0" x="15.24" y="5.08" length="short" rot="R180"/>
<pin name="CBUS3" x="15.24" y="7.62" length="short" rot="R180"/>
<wire x1="-12.7" y1="-12.7" x2="12.7" y2="-12.7" width="0.254" layer="94"/>
<wire x1="12.7" y1="-12.7" x2="12.7" y2="10.16" width="0.254" layer="94"/>
<wire x1="12.7" y1="10.16" x2="-12.7" y2="10.16" width="0.254" layer="94"/>
<wire x1="-12.7" y1="10.16" x2="-12.7" y2="-12.7" width="0.254" layer="94"/>
<text x="-12.7" y="11.43" size="1.778" layer="95">&gt;NAME</text>
<text x="-12.7" y="-15.24" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="P-CHAN-MOSET">
<pin name="S" x="2.54" y="-3.81" visible="off" length="point" rot="R270"/>
<pin name="D" x="2.54" y="1.27" visible="off" length="point" rot="R90"/>
<pin name="G" x="-2.54" y="-2.54" visible="off" length="point" rot="R180"/>
<circle x="-2.54" y="-2.54" radius="0.70995" width="0.254" layer="94"/>
<circle x="2.54" y="1.27" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-3.81" radius="0.70995" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-0.9525" x2="0.3175" y2="-1.27" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-1.27" x2="0.3175" y2="-1.5875" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-2.2225" x2="0.3175" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-2.54" x2="0.3175" y2="-2.8575" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-0.3175" x2="0.3175" y2="0" width="0.254" layer="94"/>
<wire x1="0.3175" y1="0" x2="0.3175" y2="0.3175" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-0.3175" y2="-2.54" width="0.254" layer="94"/>
<wire x1="-0.3175" y1="-2.54" x2="-0.3175" y2="0.3175" width="0.254" layer="94"/>
<wire x1="0.3175" y1="0" x2="2.54" y2="0" width="0.254" layer="94"/>
<wire x1="2.54" y1="0" x2="2.54" y2="1.27" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-2.54" x2="2.54" y2="-2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="-3.81" width="0.254" layer="94"/>
<wire x1="0.3175" y1="-1.27" x2="1.905" y2="-1.27" width="0.254" layer="94"/>
<wire x1="1.905" y1="-1.27" x2="2.54" y2="-1.27" width="0.254" layer="94"/>
<wire x1="2.54" y1="-1.27" x2="2.54" y2="-2.54" width="0.254" layer="94"/>
<wire x1="1.905" y1="-1.27" x2="1.5875" y2="-0.9525" width="0.254" layer="94"/>
<wire x1="1.5875" y1="-0.9525" x2="1.5875" y2="-1.5875" width="0.254" layer="94"/>
<wire x1="1.5875" y1="-1.5875" x2="1.905" y2="-1.27" width="0.254" layer="94"/>
<text x="-1.27" y="-0.635" size="1.778" layer="94" rot="R90">&gt;NAME</text>
<text x="5.715" y="-6.6675" size="1.778" layer="94" rot="R90">&gt;VALUE</text>
</symbol>
<symbol name="N-CHAN-MOSFET">
<wire x1="-0.635" y1="0.635" x2="0" y2="0.635" width="0.254" layer="94"/>
<wire x1="0" y1="0.635" x2="0.635" y2="0.635" width="0.254" layer="94"/>
<wire x1="1.27" y1="0.635" x2="1.905" y2="0.635" width="0.254" layer="94"/>
<wire x1="1.905" y1="0.635" x2="2.54" y2="0.635" width="0.254" layer="94"/>
<wire x1="-1.27" y1="0.635" x2="-1.905" y2="0.635" width="0.254" layer="94"/>
<wire x1="-1.905" y1="0.635" x2="-2.54" y2="0.635" width="0.254" layer="94"/>
<wire x1="-2.54" y1="1.27" x2="2.54" y2="1.27" width="0.254" layer="94"/>
<wire x1="1.905" y1="0.635" x2="1.905" y2="-1.27" width="0.254" layer="94"/>
<wire x1="1.905" y1="-1.27" x2="5.08" y2="-1.27" width="0.254" layer="94"/>
<wire x1="-1.905" y1="0.635" x2="-1.905" y2="-1.27" width="0.254" layer="94"/>
<wire x1="-1.905" y1="-1.27" x2="-5.08" y2="-1.27" width="0.254" layer="94"/>
<wire x1="-1.905" y1="-1.27" x2="0" y2="-1.27" width="0.254" layer="94"/>
<wire x1="0" y1="-1.27" x2="0" y2="0.635" width="0.254" layer="94"/>
<wire x1="-0.635" y1="0" x2="0" y2="0.635" width="0.254" layer="94"/>
<wire x1="0" y1="0.635" x2="0.635" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="1.27" x2="-2.54" y2="3.81" width="0.254" layer="94"/>
<pin name="G" x="-2.54" y="3.81" visible="pin" length="point" rot="R180"/>
<pin name="S" x="-5.08" y="-1.27" visible="pin" length="point" rot="R270"/>
<pin name="D" x="5.08" y="-1.27" visible="pin" length="point" rot="R270"/>
<text x="1.27" y="2.54" size="1.778" layer="94">&gt;NAME</text>
<text x="1.27" y="5.08" size="1.778" layer="94">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="FT230X" prefix="IC">
<description>&lt;b&gt;USB to BASIC UART IC&lt;/b&gt;&lt;p&gt;
RS 757-0010</description>
<gates>
<gate name="G$1" symbol="FT230X" x="0" y="0"/>
</gates>
<devices>
<device name="S" package="SSOP16">
<connects>
<connect gate="G$1" pin="!CTS" pad="6"/>
<connect gate="G$1" pin="!RESET" pad="11"/>
<connect gate="G$1" pin="!RTS" pad="2"/>
<connect gate="G$1" pin="3V3OUT" pad="10"/>
<connect gate="G$1" pin="CBUS0" pad="15"/>
<connect gate="G$1" pin="CBUS1" pad="14"/>
<connect gate="G$1" pin="CBUS2" pad="7"/>
<connect gate="G$1" pin="CBUS3" pad="16"/>
<connect gate="G$1" pin="GND" pad="5"/>
<connect gate="G$1" pin="GND@1" pad="13"/>
<connect gate="G$1" pin="RXD" pad="4"/>
<connect gate="G$1" pin="TXD" pad="1"/>
<connect gate="G$1" pin="USBDM" pad="9"/>
<connect gate="G$1" pin="USBDP" pad="8"/>
<connect gate="G$1" pin="VCC" pad="12"/>
<connect gate="G$1" pin="VCCIO" pad="3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="Q" package="QFN16">
<connects>
<connect gate="G$1" pin="!CTS" pad="4"/>
<connect gate="G$1" pin="!RESET" pad="9"/>
<connect gate="G$1" pin="!RTS" pad="16"/>
<connect gate="G$1" pin="3V3OUT" pad="8"/>
<connect gate="G$1" pin="CBUS0" pad="12"/>
<connect gate="G$1" pin="CBUS1" pad="11"/>
<connect gate="G$1" pin="CBUS2" pad="5"/>
<connect gate="G$1" pin="CBUS3" pad="14"/>
<connect gate="G$1" pin="GND" pad="3"/>
<connect gate="G$1" pin="GND@1" pad="13"/>
<connect gate="G$1" pin="RXD" pad="2"/>
<connect gate="G$1" pin="TXD" pad="15"/>
<connect gate="G$1" pin="USBDM" pad="7"/>
<connect gate="G$1" pin="USBDP" pad="6"/>
<connect gate="G$1" pin="VCC" pad="10"/>
<connect gate="G$1" pin="VCCIO" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="BSS84" prefix="TR" uservalue="yes">
<description>RS 436-8091P</description>
<gates>
<gate name="G$1" symbol="P-CHAN-MOSET" x="-45.72" y="27.94"/>
</gates>
<devices>
<device name="" package="SOT23">
<connects>
<connect gate="G$1" pin="D" pad="3"/>
<connect gate="G$1" pin="G" pad="1"/>
<connect gate="G$1" pin="S" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="2SK3018" prefix="TR">
<description>SM equivalent of 2N7000 RS 694-2812</description>
<gates>
<gate name="G$1" symbol="N-CHAN-MOSFET" x="-25.4" y="12.7"/>
</gates>
<devices>
<device name="" package="2SK3018">
<connects>
<connect gate="G$1" pin="D" pad="3"/>
<connect gate="G$1" pin="G" pad="2"/>
<connect gate="G$1" pin="S" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="7_SEG2" library="1_marko" deviceset="4_DIGIT_7SEGMENT_DISPLAY" device=""/>
<part name="7_SEG1" library="1_marko" deviceset="4_DIGIT_7SEGMENT_DISPLAY" device=""/>
<part name="IC3" library="1_Home_library" deviceset="MEGA8-P" device=""/>
<part name="SW2" library="1_Home_library" deviceset="NO_SW" device=""/>
<part name="GND11" library="supply1" deviceset="0V" device=""/>
<part name="TR3" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR6" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR2" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR7" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR4" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR8" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR5" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="TR9" library="1_3V3_Bootloader" deviceset="BSS84" device="" value="BSS84"/>
<part name="R24" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R20" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R25" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R26" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R21" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R22" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="R23" library="1_Home_library" deviceset="1206R" device="" value="220"/>
<part name="P+12" library="supply1" deviceset="+5V" device=""/>
<part name="P+7" library="supply1" deviceset="+5V" device=""/>
<part name="P+9" library="supply1" deviceset="+5V" device=""/>
<part name="GND10" library="supply1" deviceset="0V" device=""/>
<part name="GND9" library="supply1" deviceset="0V" device=""/>
<part name="R14" library="1_Home_library" deviceset="1206R" device="" value="10K"/>
<part name="P+8" library="supply1" deviceset="+5V" device=""/>
<part name="IC1" library="1_3V3_Bootloader" deviceset="FT230X" device="S"/>
<part name="C6" library="1_Home_library" deviceset="1206C" device="" value="100n"/>
<part name="C5" library="1_Home_library" deviceset="1206C" device="" value="47p"/>
<part name="C2" library="1_Home_library" deviceset="1206C" device="" value="47p"/>
<part name="C3" library="1_Home_library" deviceset="1206C" device="" value="100n"/>
<part name="R1" library="1_Home_library" deviceset="1206R" device="" value="27R"/>
<part name="R4" library="1_Home_library" deviceset="1206R" device="" value="27R"/>
<part name="SKT1" library="3_My_prog" deviceset="USB-B" device=""/>
<part name="L1" library="1_Home_library" deviceset="CHOKE" device=""/>
<part name="C1" library="1_Home_library" deviceset="1206C" device="" value="10n"/>
<part name="C4" library="1_Home_library" deviceset="1206C" device="" value="4u7"/>
<part name="GND1" library="supply1" deviceset="0V" device=""/>
<part name="P+1" library="supply1" deviceset="+5V" device=""/>
<part name="GND2" library="supply1" deviceset="0V" device=""/>
<part name="R6" library="1_Home_library" deviceset="1206R" device="" value="3k9"/>
<part name="R2" library="1_Home_library" deviceset="1206R" device="" value="10K"/>
<part name="R3" library="1_Home_library" deviceset="1206R" device="" value="10K"/>
<part name="TR10" library="1_3V3_Bootloader" deviceset="2SK3018" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="58.42" y="78.74" size="3.81" layer="97" rot="R90">ATMEGA 328</text>
<text x="194.31" y="105.41" size="2.54" layer="92" font="vector">KINGSBRIGHT
CA56-12SYKWA
CA56- 12SURKWA
or similar</text>
<text x="-106.68" y="172.72" size="3.048" layer="92">Multiplexer Operation

To illuminate a digit its input must be high
To display a number the appropriate letters must be low
For example to display -2- the letters are a, b,g,e and d 

A 2ms interrupt is used to update the drive signals to the digits
and letters so that each digit is illuminated in turn and also so that
it displays the correct number.  </text>
<text x="193.04" y="93.98" size="2.54" layer="92">Mount 7_SEG1 to the
left of 7_SEG2</text>
</plain>
<instances>
<instance part="7_SEG2" gate="G$1" x="201.93" y="151.13" smashed="yes">
<attribute name="NAME" x="201.93" y="132.08" size="1.778" layer="94"/>
<attribute name="VALUE" x="217.17" y="128.27" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="7_SEG1" gate="G$1" x="201.93" y="198.12" smashed="yes">
<attribute name="NAME" x="203.2" y="179.07" size="1.778" layer="94"/>
</instance>
<instance part="IC3" gate="G$1" x="60.96" y="107.95"/>
<instance part="SW2" gate="G$1" x="49.53" y="166.37" rot="R90"/>
<instance part="GND11" gate="1" x="49.53" y="196.85" rot="R180"/>
<instance part="TR3" gate="G$1" x="129.54" y="113.03" smashed="yes" rot="MR180">
<attribute name="NAME" x="122.555" y="123.19" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="121.6025" y="120.015" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR6" gate="G$1" x="130.81" y="99.06" smashed="yes" rot="MR180">
<attribute name="NAME" x="126.365" y="106.68" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="127.9525" y="95.885" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR2" gate="G$1" x="148.59" y="88.9" smashed="yes" rot="MR180">
<attribute name="NAME" x="141.605" y="88.9" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="141.9225" y="85.725" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR7" gate="G$1" x="130.81" y="77.47" smashed="yes" rot="MR180">
<attribute name="NAME" x="122.555" y="78.74" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="119.0625" y="75.565" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR4" gate="G$1" x="129.54" y="156.21" smashed="yes" rot="MR180">
<attribute name="NAME" x="121.285" y="157.48" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="120.3325" y="161.925" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR8" gate="G$1" x="147.32" y="144.78" smashed="yes" rot="MR180">
<attribute name="NAME" x="141.605" y="152.4" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="149.5425" y="151.765" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR5" gate="G$1" x="128.27" y="139.7" smashed="yes" rot="MR180">
<attribute name="NAME" x="121.285" y="139.7" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="120.3325" y="136.525" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="TR9" gate="G$1" x="147.32" y="128.27" smashed="yes" rot="MR180">
<attribute name="NAME" x="141.605" y="134.62" size="1.778" layer="94" rot="MR180"/>
<attribute name="VALUE" x="139.3825" y="126.365" size="1.778" layer="94" rot="MR180"/>
</instance>
<instance part="R24" gate="G$1" x="118.11" y="210.82" smashed="yes" rot="R180">
<attribute name="VALUE" x="124.968" y="212.852" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="114.808" y="213.106" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R20" gate="G$1" x="133.35" y="200.66" smashed="yes" rot="R180">
<attribute name="VALUE" x="141.478" y="198.882" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="130.048" y="199.136" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R25" gate="G$1" x="125.73" y="190.5" smashed="yes" rot="R180">
<attribute name="VALUE" x="131.318" y="188.722" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="121.158" y="188.976" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R26" gate="G$1" x="134.62" y="182.88" smashed="yes" rot="R180">
<attribute name="VALUE" x="141.478" y="181.102" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="130.048" y="181.356" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R21" gate="G$1" x="148.59" y="176.53" smashed="yes" rot="R180">
<attribute name="VALUE" x="155.448" y="174.752" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="144.018" y="175.006" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R22" gate="G$1" x="124.46" y="204.47" smashed="yes" rot="R180">
<attribute name="VALUE" x="131.318" y="206.502" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="121.158" y="206.756" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R23" gate="G$1" x="146.05" y="195.58" smashed="yes" rot="R180">
<attribute name="VALUE" x="151.638" y="193.802" size="1.778" layer="94" rot="R180"/>
<attribute name="NAME" x="142.748" y="194.056" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="P+12" gate="1" x="193.04" y="69.85" rot="R270"/>
<instance part="P+7" gate="1" x="33.02" y="125.73" rot="R90"/>
<instance part="P+9" gate="1" x="38.1" y="92.71" rot="R180"/>
<instance part="GND10" gate="1" x="34.29" y="92.71"/>
<instance part="GND9" gate="1" x="34.29" y="130.81" rot="R270"/>
<instance part="R14" gate="G$1" x="34.29" y="166.37" rot="R90"/>
<instance part="P+8" gate="1" x="34.29" y="180.34"/>
<instance part="IC1" gate="G$1" x="-36.83" y="104.14" smashed="yes">
<attribute name="NAME" x="-49.53" y="115.57" size="1.778" layer="95"/>
<attribute name="VALUE" x="-43.18" y="115.57" size="1.778" layer="96"/>
</instance>
<instance part="C6" gate="G$1" x="-10.16" y="80.01" rot="R90"/>
<instance part="C5" gate="G$1" x="-39.37" y="77.47" rot="R90"/>
<instance part="C2" gate="G$1" x="-69.85" y="76.2" rot="R90"/>
<instance part="C3" gate="G$1" x="-68.58" y="116.84" smashed="yes" rot="R90">
<attribute name="VALUE" x="-69.85" y="119.38" size="1.27" layer="96" rot="R90"/>
<attribute name="NAME" x="-71.12" y="114.3" size="1.778" layer="95" rot="R90"/>
</instance>
<instance part="R1" gate="G$1" x="-63.5" y="93.98" rot="R180"/>
<instance part="R4" gate="G$1" x="-29.21" y="82.55" rot="R180"/>
<instance part="SKT1" gate="G$1" x="-87.63" y="91.44" rot="MR0"/>
<instance part="L1" gate="G$1" x="-77.47" y="110.49" rot="R90"/>
<instance part="C1" gate="G$1" x="-77.47" y="76.2" smashed="yes" rot="R90">
<attribute name="VALUE" x="-78.74" y="78.74" size="1.27" layer="96" rot="R90"/>
<attribute name="NAME" x="-80.01" y="73.66" size="1.778" layer="95" rot="R90"/>
</instance>
<instance part="C4" gate="G$1" x="-60.96" y="116.84" rot="R90"/>
<instance part="GND1" gate="1" x="-64.77" y="106.68"/>
<instance part="P+1" gate="1" x="-27.94" y="166.37"/>
<instance part="GND2" gate="1" x="-55.88" y="66.04" rot="MR0"/>
<instance part="R6" gate="G$1" x="-2.54" y="77.47" rot="R90"/>
<instance part="R2" gate="G$1" x="-52.07" y="143.51" rot="R90"/>
<instance part="R3" gate="G$1" x="-35.56" y="152.4" rot="R90"/>
<instance part="TR10" gate="G$1" x="-43.18" y="138.43" smashed="yes">
<attribute name="NAME" x="-41.91" y="140.97" size="1.778" layer="94"/>
<attribute name="VALUE" x="-48.26" y="130.81" size="1.778" layer="94"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="0V" class="0">
<segment>
<pinref part="SW2" gate="G$1" pin="2"/>
<pinref part="GND11" gate="1" pin="0V"/>
<wire x1="49.53" y1="176.53" x2="49.53" y2="194.31" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND9" gate="1" pin="0V"/>
<pinref part="IC3" gate="G$1" pin="GND"/>
<wire x1="36.83" y1="130.81" x2="38.1" y2="130.81" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C3" gate="G$1" pin="1"/>
<wire x1="-68.58" y1="111.76" x2="-68.58" y2="110.49" width="0.1524" layer="91"/>
<wire x1="-68.58" y1="110.49" x2="-64.77" y2="110.49" width="0.1524" layer="91"/>
<pinref part="C4" gate="G$1" pin="1"/>
<wire x1="-64.77" y1="110.49" x2="-60.96" y2="110.49" width="0.1524" layer="91"/>
<wire x1="-60.96" y1="110.49" x2="-60.96" y2="111.76" width="0.1524" layer="91"/>
<pinref part="GND1" gate="1" pin="0V"/>
<wire x1="-64.77" y1="109.22" x2="-64.77" y2="110.49" width="0.1524" layer="91"/>
<junction x="-64.77" y="110.49"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="GND"/>
<wire x1="-52.07" y1="101.6" x2="-73.66" y2="101.6" width="0.1524" layer="91"/>
<wire x1="-73.66" y1="101.6" x2="-73.66" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-73.66" y1="71.12" x2="-55.88" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-55.88" y1="71.12" x2="-39.37" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-39.37" y1="71.12" x2="-19.05" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-19.05" y1="71.12" x2="-19.05" y2="104.14" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="GND@1"/>
<wire x1="-19.05" y1="104.14" x2="-21.59" y2="104.14" width="0.1524" layer="91"/>
<pinref part="C6" gate="G$1" pin="1"/>
<wire x1="-10.16" y1="74.93" x2="-10.16" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="71.12" x2="-19.05" y2="71.12" width="0.1524" layer="91"/>
<junction x="-19.05" y="71.12"/>
<junction x="-73.66" y="71.12"/>
<pinref part="C2" gate="G$1" pin="1"/>
<pinref part="C5" gate="G$1" pin="1"/>
<wire x1="-39.37" y1="72.39" x2="-39.37" y2="71.12" width="0.1524" layer="91"/>
<junction x="-39.37" y="71.12"/>
<pinref part="SKT1" gate="G$1" pin="GND"/>
<wire x1="-85.09" y1="91.44" x2="-83.82" y2="91.44" width="0.1524" layer="91"/>
<wire x1="-83.82" y1="91.44" x2="-83.82" y2="88.9" width="0.1524" layer="91"/>
<pinref part="SKT1" gate="G$1" pin="GND@2"/>
<wire x1="-83.82" y1="88.9" x2="-83.82" y2="86.36" width="0.1524" layer="91"/>
<wire x1="-83.82" y1="86.36" x2="-85.09" y2="86.36" width="0.1524" layer="91"/>
<pinref part="SKT1" gate="G$1" pin="GND@1"/>
<wire x1="-85.09" y1="88.9" x2="-83.82" y2="88.9" width="0.1524" layer="91"/>
<junction x="-83.82" y="88.9"/>
<wire x1="-83.82" y1="86.36" x2="-83.82" y2="71.12" width="0.1524" layer="91"/>
<junction x="-83.82" y="86.36"/>
<pinref part="C1" gate="G$1" pin="1"/>
<wire x1="-83.82" y1="71.12" x2="-77.47" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-69.85" y1="71.12" x2="-73.66" y2="71.12" width="0.1524" layer="91"/>
<junction x="-69.85" y="71.12"/>
<junction x="-77.47" y="71.12"/>
<wire x1="-73.66" y1="71.12" x2="-77.47" y2="71.12" width="0.1524" layer="91"/>
<pinref part="GND2" gate="1" pin="0V"/>
<wire x1="-55.88" y1="68.58" x2="-55.88" y2="71.12" width="0.1524" layer="91"/>
<junction x="-55.88" y="71.12"/>
<pinref part="R6" gate="G$1" pin="P$1"/>
<wire x1="-2.54" y1="72.39" x2="-2.54" y2="71.12" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="71.12" x2="-10.16" y2="71.12" width="0.1524" layer="91"/>
<junction x="-10.16" y="71.12"/>
</segment>
<segment>
<pinref part="GND10" gate="1" pin="0V"/>
<wire x1="34.29" y1="95.25" x2="34.29" y2="105.41" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="GND@1"/>
<wire x1="34.29" y1="105.41" x2="38.1" y2="105.41" width="0.1524" layer="91"/>
</segment>
</net>
<net name="+5V" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="AVCC"/>
<pinref part="P+7" gate="1" pin="+5V"/>
<wire x1="38.1" y1="125.73" x2="35.56" y2="125.73" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="IC3" gate="G$1" pin="VCC@1"/>
<pinref part="P+9" gate="1" pin="+5V"/>
<wire x1="38.1" y1="100.33" x2="38.1" y2="95.25" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R14" gate="G$1" pin="P$2"/>
<pinref part="P+8" gate="1" pin="+5V"/>
<wire x1="34.29" y1="171.45" x2="34.29" y2="177.8" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="VCC"/>
<wire x1="-21.59" y1="101.6" x2="-13.97" y2="101.6" width="0.1524" layer="91"/>
<wire x1="-13.97" y1="101.6" x2="-13.97" y2="124.46" width="0.1524" layer="91"/>
<pinref part="L1" gate="G$1" pin="2"/>
<wire x1="-13.97" y1="124.46" x2="-27.94" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-27.94" y1="124.46" x2="-60.96" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-60.96" y1="124.46" x2="-68.58" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-68.58" y1="124.46" x2="-77.47" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-77.47" y1="124.46" x2="-77.47" y2="115.57" width="0.1524" layer="91"/>
<pinref part="C4" gate="G$1" pin="2"/>
<wire x1="-60.96" y1="121.92" x2="-60.96" y2="124.46" width="0.1524" layer="91"/>
<junction x="-60.96" y="124.46"/>
<pinref part="C3" gate="G$1" pin="2"/>
<wire x1="-68.58" y1="121.92" x2="-68.58" y2="124.46" width="0.1524" layer="91"/>
<junction x="-68.58" y="124.46"/>
<pinref part="P+1" gate="1" pin="+5V"/>
<wire x1="-27.94" y1="163.83" x2="-27.94" y2="161.29" width="0.1524" layer="91"/>
<junction x="-27.94" y="124.46"/>
<wire x1="-27.94" y1="161.29" x2="-27.94" y2="124.46" width="0.1524" layer="91"/>
<pinref part="R3" gate="G$1" pin="P$2"/>
<wire x1="-35.56" y1="157.48" x2="-35.56" y2="161.29" width="0.1524" layer="91"/>
<wire x1="-35.56" y1="161.29" x2="-27.94" y2="161.29" width="0.1524" layer="91"/>
<junction x="-27.94" y="161.29"/>
</segment>
<segment>
<pinref part="TR4" gate="G$1" pin="S"/>
<wire x1="132.08" y1="160.02" x2="137.16" y2="160.02" width="0.1524" layer="91"/>
<wire x1="137.16" y1="160.02" x2="137.16" y2="143.51" width="0.1524" layer="91"/>
<wire x1="137.16" y1="143.51" x2="137.16" y2="116.84" width="0.1524" layer="91"/>
<wire x1="137.16" y1="116.84" x2="137.16" y2="102.87" width="0.1524" layer="91"/>
<wire x1="137.16" y1="102.87" x2="137.16" y2="81.28" width="0.1524" layer="91"/>
<wire x1="137.16" y1="81.28" x2="137.16" y2="69.85" width="0.1524" layer="91"/>
<pinref part="TR5" gate="G$1" pin="S"/>
<wire x1="130.81" y1="143.51" x2="137.16" y2="143.51" width="0.1524" layer="91"/>
<junction x="137.16" y="143.51"/>
<pinref part="TR3" gate="G$1" pin="S"/>
<wire x1="132.08" y1="116.84" x2="137.16" y2="116.84" width="0.1524" layer="91"/>
<junction x="137.16" y="116.84"/>
<pinref part="TR6" gate="G$1" pin="S"/>
<wire x1="133.35" y1="102.87" x2="137.16" y2="102.87" width="0.1524" layer="91"/>
<junction x="137.16" y="102.87"/>
<pinref part="TR7" gate="G$1" pin="S"/>
<wire x1="133.35" y1="81.28" x2="137.16" y2="81.28" width="0.1524" layer="91"/>
<junction x="137.16" y="81.28"/>
<wire x1="137.16" y1="69.85" x2="156.21" y2="69.85" width="0.1524" layer="91"/>
<pinref part="TR8" gate="G$1" pin="S"/>
<wire x1="156.21" y1="148.59" x2="149.86" y2="148.59" width="0.1524" layer="91"/>
<pinref part="TR2" gate="G$1" pin="S"/>
<wire x1="151.13" y1="92.71" x2="156.21" y2="92.71" width="0.1524" layer="91"/>
<wire x1="156.21" y1="92.71" x2="156.21" y2="132.08" width="0.1524" layer="91"/>
<wire x1="156.21" y1="132.08" x2="156.21" y2="148.59" width="0.1524" layer="91"/>
<junction x="156.21" y="132.08"/>
<pinref part="TR9" gate="G$1" pin="S"/>
<wire x1="149.86" y1="132.08" x2="156.21" y2="132.08" width="0.1524" layer="91"/>
<wire x1="156.21" y1="69.85" x2="156.21" y2="92.71" width="0.1524" layer="91"/>
<junction x="156.21" y="92.71"/>
<pinref part="P+12" gate="1" pin="+5V"/>
<wire x1="156.21" y1="69.85" x2="190.5" y2="69.85" width="0.1524" layer="91"/>
<junction x="156.21" y="69.85"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<wire x1="-5.08" y1="132.08" x2="10.16" y2="132.08" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD1(TXD)"/>
<wire x1="86.36" y1="110.49" x2="90.17" y2="110.49" width="0.1524" layer="91"/>
<wire x1="90.17" y1="110.49" x2="90.17" y2="146.05" width="0.1524" layer="91"/>
<wire x1="90.17" y1="146.05" x2="10.16" y2="146.05" width="0.1524" layer="91"/>
<wire x1="10.16" y1="146.05" x2="10.16" y2="132.08" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="RXD"/>
<wire x1="-52.07" y1="104.14" x2="-55.88" y2="104.14" width="0.1524" layer="91"/>
<wire x1="-55.88" y1="104.14" x2="-55.88" y2="86.36" width="0.1524" layer="91"/>
<wire x1="-55.88" y1="86.36" x2="-5.08" y2="86.36" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="86.36" x2="-5.08" y2="132.08" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$31" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PC6(/RESET)"/>
<wire x1="38.1" y1="135.89" x2="34.29" y2="135.89" width="0.1524" layer="91"/>
<wire x1="34.29" y1="135.89" x2="34.29" y2="156.21" width="0.1524" layer="91"/>
<wire x1="34.29" y1="156.21" x2="49.53" y2="156.21" width="0.1524" layer="91"/>
<wire x1="49.53" y1="156.21" x2="49.53" y2="163.83" width="0.1524" layer="91"/>
<pinref part="SW2" gate="G$1" pin="1"/>
<pinref part="R14" gate="G$1" pin="P$1"/>
<wire x1="34.29" y1="156.21" x2="34.29" y2="161.29" width="0.1524" layer="91"/>
<junction x="34.29" y="156.21"/>
</segment>
</net>
<net name="SEVEN_SEG0" class="0">
<segment>
<pinref part="R24" gate="G$1" pin="P$1"/>
<pinref part="7_SEG1" gate="G$1" pin="A"/>
<wire x1="199.39" y1="210.82" x2="196.85" y2="210.82" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="A"/>
<wire x1="196.85" y1="210.82" x2="123.19" y2="210.82" width="0.1524" layer="91"/>
<wire x1="199.39" y1="163.83" x2="196.85" y2="163.83" width="0.1524" layer="91"/>
<wire x1="196.85" y1="210.82" x2="196.85" y2="163.83" width="0.1524" layer="91"/>
<junction x="196.85" y="210.82"/>
</segment>
</net>
<net name="SEVEN_SEG1" class="0">
<segment>
<pinref part="7_SEG1" gate="G$1" pin="B"/>
<wire x1="199.39" y1="200.66" x2="193.04" y2="200.66" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="B"/>
<wire x1="199.39" y1="153.67" x2="193.04" y2="153.67" width="0.1524" layer="91"/>
<pinref part="R20" gate="G$1" pin="P$1"/>
<wire x1="193.04" y1="200.66" x2="193.04" y2="153.67" width="0.1524" layer="91"/>
<wire x1="138.43" y1="200.66" x2="193.04" y2="200.66" width="0.1524" layer="91"/>
<junction x="193.04" y="200.66"/>
</segment>
</net>
<net name="SEVEN_SEG2" class="0">
<segment>
<pinref part="7_SEG2" gate="G$1" pin="C"/>
<wire x1="199.39" y1="146.05" x2="189.23" y2="146.05" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="C"/>
<wire x1="189.23" y1="193.04" x2="199.39" y2="193.04" width="0.1524" layer="91"/>
<wire x1="189.23" y1="146.05" x2="189.23" y2="190.5" width="0.1524" layer="91"/>
<pinref part="R25" gate="G$1" pin="P$1"/>
<wire x1="189.23" y1="190.5" x2="189.23" y2="193.04" width="0.1524" layer="91"/>
<wire x1="130.81" y1="190.5" x2="189.23" y2="190.5" width="0.1524" layer="91"/>
<junction x="189.23" y="190.5"/>
</segment>
</net>
<net name="SEVEN_SEG3" class="0">
<segment>
<pinref part="7_SEG1" gate="G$1" pin="D"/>
<wire x1="199.39" y1="187.96" x2="184.15" y2="187.96" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="D"/>
<wire x1="199.39" y1="140.97" x2="184.15" y2="140.97" width="0.1524" layer="91"/>
<wire x1="184.15" y1="187.96" x2="184.15" y2="182.88" width="0.1524" layer="91"/>
<pinref part="R26" gate="G$1" pin="P$1"/>
<wire x1="184.15" y1="182.88" x2="184.15" y2="140.97" width="0.1524" layer="91"/>
<wire x1="139.7" y1="182.88" x2="184.15" y2="182.88" width="0.1524" layer="91"/>
<junction x="184.15" y="182.88"/>
</segment>
</net>
<net name="SEVEN_SEG4" class="0">
<segment>
<pinref part="7_SEG2" gate="G$1" pin="E"/>
<wire x1="199.39" y1="138.43" x2="186.69" y2="138.43" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="E"/>
<wire x1="199.39" y1="185.42" x2="186.69" y2="185.42" width="0.1524" layer="91"/>
<wire x1="186.69" y1="138.43" x2="186.69" y2="176.53" width="0.1524" layer="91"/>
<pinref part="R21" gate="G$1" pin="P$1"/>
<wire x1="186.69" y1="176.53" x2="186.69" y2="185.42" width="0.1524" layer="91"/>
<wire x1="153.67" y1="176.53" x2="186.69" y2="176.53" width="0.1524" layer="91"/>
<junction x="186.69" y="176.53"/>
</segment>
</net>
<net name="SEVEN_SEG5" class="0">
<segment>
<pinref part="7_SEG1" gate="G$1" pin="F"/>
<wire x1="199.39" y1="208.28" x2="194.31" y2="208.28" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="F"/>
<wire x1="199.39" y1="161.29" x2="194.31" y2="161.29" width="0.1524" layer="91"/>
<pinref part="R22" gate="G$1" pin="P$1"/>
<wire x1="194.31" y1="208.28" x2="194.31" y2="161.29" width="0.1524" layer="91"/>
<wire x1="129.54" y1="204.47" x2="147.32" y2="204.47" width="0.1524" layer="91"/>
<wire x1="147.32" y1="204.47" x2="147.32" y2="208.28" width="0.1524" layer="91"/>
<wire x1="147.32" y1="208.28" x2="194.31" y2="208.28" width="0.1524" layer="91"/>
<junction x="194.31" y="208.28"/>
</segment>
</net>
<net name="SEVEN_SEG6" class="0">
<segment>
<pinref part="7_SEG1" gate="G$1" pin="G"/>
<wire x1="199.39" y1="195.58" x2="190.5" y2="195.58" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="G"/>
<wire x1="199.39" y1="148.59" x2="190.5" y2="148.59" width="0.1524" layer="91"/>
<pinref part="R23" gate="G$1" pin="P$1"/>
<wire x1="190.5" y1="195.58" x2="190.5" y2="148.59" width="0.1524" layer="91"/>
<wire x1="151.13" y1="195.58" x2="190.5" y2="195.58" width="0.1524" layer="91"/>
<junction x="190.5" y="195.58"/>
</segment>
</net>
<net name="DISPLAY_DRIVER0" class="0">
<segment>
<pinref part="R24" gate="G$1" pin="P$2"/>
<wire x1="113.03" y1="210.82" x2="109.22" y2="210.82" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PB1(OC1A)"/>
<wire x1="86.36" y1="87.63" x2="109.22" y2="87.63" width="0.1524" layer="91"/>
<wire x1="109.22" y1="210.82" x2="109.22" y2="87.63" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER1" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PD2(INT0)"/>
<wire x1="86.36" y1="107.95" x2="111.76" y2="107.95" width="0.1524" layer="91"/>
<pinref part="R20" gate="G$1" pin="P$2"/>
<wire x1="128.27" y1="200.66" x2="111.76" y2="200.66" width="0.1524" layer="91"/>
<wire x1="111.76" y1="200.66" x2="111.76" y2="107.95" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER2" class="0">
<segment>
<pinref part="R25" gate="G$1" pin="P$2"/>
<wire x1="120.65" y1="190.5" x2="114.3" y2="190.5" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD3(INT1)"/>
<wire x1="86.36" y1="105.41" x2="114.3" y2="105.41" width="0.1524" layer="91"/>
<wire x1="114.3" y1="190.5" x2="114.3" y2="105.41" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER3" class="0">
<segment>
<pinref part="R26" gate="G$1" pin="P$2"/>
<wire x1="129.54" y1="182.88" x2="115.57" y2="182.88" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD4(XCK/T0)"/>
<wire x1="86.36" y1="102.87" x2="115.57" y2="102.87" width="0.1524" layer="91"/>
<wire x1="115.57" y1="182.88" x2="115.57" y2="102.87" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER4" class="0">
<segment>
<pinref part="R21" gate="G$1" pin="P$2"/>
<wire x1="143.51" y1="176.53" x2="116.84" y2="176.53" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD5(T1)"/>
<wire x1="86.36" y1="100.33" x2="116.84" y2="100.33" width="0.1524" layer="91"/>
<wire x1="116.84" y1="176.53" x2="116.84" y2="100.33" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER5" class="0">
<segment>
<pinref part="R22" gate="G$1" pin="P$2"/>
<wire x1="119.38" y1="204.47" x2="110.49" y2="204.47" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD6(AIN0)"/>
<wire x1="86.36" y1="97.79" x2="110.49" y2="97.79" width="0.1524" layer="91"/>
<wire x1="110.49" y1="204.47" x2="110.49" y2="97.79" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER6" class="0">
<segment>
<pinref part="R23" gate="G$1" pin="P$2"/>
<wire x1="140.97" y1="195.58" x2="113.03" y2="195.58" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PD7(AIN1)"/>
<wire x1="86.36" y1="95.25" x2="113.03" y2="95.25" width="0.1524" layer="91"/>
<wire x1="113.03" y1="195.58" x2="113.03" y2="95.25" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER8" class="0">
<segment>
<wire x1="123.19" y1="90.17" x2="86.36" y2="90.17" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PB0(ICP)"/>
<wire x1="123.19" y1="90.17" x2="123.19" y2="115.57" width="0.1524" layer="91"/>
<pinref part="TR3" gate="G$1" pin="G"/>
<wire x1="123.19" y1="115.57" x2="127" y2="115.57" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER9" class="0">
<segment>
<pinref part="TR6" gate="G$1" pin="G"/>
<wire x1="124.46" y1="101.6" x2="128.27" y2="101.6" width="0.1524" layer="91"/>
<wire x1="124.46" y1="101.6" x2="124.46" y2="85.09" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PB2(SS/OC1B)"/>
<wire x1="86.36" y1="85.09" x2="124.46" y2="85.09" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER10" class="0">
<segment>
<wire x1="125.73" y1="82.55" x2="86.36" y2="82.55" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PB3(MOSI/OC2)"/>
<wire x1="125.73" y1="82.55" x2="125.73" y2="91.44" width="0.1524" layer="91"/>
<wire x1="125.73" y1="91.44" x2="146.05" y2="91.44" width="0.1524" layer="91"/>
<pinref part="TR2" gate="G$1" pin="G"/>
</segment>
</net>
<net name="DISPLAY_DRIVER11" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PB4(MISO)"/>
<wire x1="86.36" y1="80.01" x2="128.27" y2="80.01" width="0.1524" layer="91"/>
<pinref part="TR7" gate="G$1" pin="G"/>
</segment>
</net>
<net name="DISPLAY_DRIVER12" class="0">
<segment>
<pinref part="TR4" gate="G$1" pin="G"/>
<wire x1="93.98" y1="158.75" x2="127" y2="158.75" width="0.1524" layer="91"/>
<wire x1="93.98" y1="158.75" x2="93.98" y2="77.47" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PB5(SCK)"/>
<wire x1="93.98" y1="77.47" x2="86.36" y2="77.47" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER13" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PC0(ADC0)"/>
<wire x1="95.25" y1="135.89" x2="86.36" y2="135.89" width="0.1524" layer="91"/>
<wire x1="95.25" y1="135.89" x2="95.25" y2="147.32" width="0.1524" layer="91"/>
<pinref part="TR8" gate="G$1" pin="G"/>
<wire x1="95.25" y1="147.32" x2="144.78" y2="147.32" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DISPLAY_DRIVER14" class="0">
<segment>
<pinref part="TR5" gate="G$1" pin="G"/>
<wire x1="125.73" y1="142.24" x2="96.52" y2="142.24" width="0.1524" layer="91"/>
<pinref part="IC3" gate="G$1" pin="PC1(ADC1)"/>
<wire x1="86.36" y1="133.35" x2="96.52" y2="133.35" width="0.1524" layer="91"/>
<wire x1="96.52" y1="133.35" x2="96.52" y2="142.24" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIGIT0" class="0">
<segment>
<pinref part="TR4" gate="G$1" pin="D"/>
<wire x1="132.08" y1="154.94" x2="160.02" y2="154.94" width="0.1524" layer="91"/>
<wire x1="160.02" y1="154.94" x2="160.02" y2="213.36" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="DIG1"/>
<wire x1="160.02" y1="213.36" x2="199.39" y2="213.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIGIT4" class="0">
<segment>
<pinref part="TR3" gate="G$1" pin="D"/>
<wire x1="132.08" y1="111.76" x2="171.45" y2="111.76" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="DIG1"/>
<wire x1="199.39" y1="166.37" x2="171.45" y2="166.37" width="0.1524" layer="91"/>
<wire x1="171.45" y1="111.76" x2="171.45" y2="166.37" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIGIT5" class="0">
<segment>
<pinref part="TR6" gate="G$1" pin="D"/>
<wire x1="133.35" y1="97.79" x2="173.99" y2="97.79" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="DIG2"/>
<wire x1="199.39" y1="158.75" x2="173.99" y2="158.75" width="0.1524" layer="91"/>
<wire x1="173.99" y1="97.79" x2="173.99" y2="158.75" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIGIT6" class="0">
<segment>
<pinref part="TR2" gate="G$1" pin="D"/>
<wire x1="151.13" y1="87.63" x2="175.26" y2="87.63" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="DIG3"/>
<wire x1="199.39" y1="156.21" x2="175.26" y2="156.21" width="0.1524" layer="91"/>
<wire x1="175.26" y1="87.63" x2="175.26" y2="156.21" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIGIT7" class="0">
<segment>
<pinref part="TR7" gate="G$1" pin="D"/>
<wire x1="133.35" y1="76.2" x2="176.53" y2="76.2" width="0.1524" layer="91"/>
<wire x1="176.53" y1="76.2" x2="176.53" y2="151.13" width="0.1524" layer="91"/>
<pinref part="7_SEG2" gate="G$1" pin="DIG4"/>
<wire x1="199.39" y1="151.13" x2="176.53" y2="151.13" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="!RESET"/>
<wire x1="-21.59" y1="99.06" x2="-16.51" y2="99.06" width="0.1524" layer="91"/>
<wire x1="-16.51" y1="99.06" x2="-16.51" y2="119.38" width="0.1524" layer="91"/>
<wire x1="-16.51" y1="119.38" x2="-55.88" y2="119.38" width="0.1524" layer="91"/>
<wire x1="-55.88" y1="119.38" x2="-55.88" y2="106.68" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="VCCIO"/>
<wire x1="-55.88" y1="106.68" x2="-52.07" y2="106.68" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="3V3OUT"/>
<wire x1="-21.59" y1="96.52" x2="-16.51" y2="96.52" width="0.1524" layer="91"/>
<wire x1="-16.51" y1="96.52" x2="-16.51" y2="99.06" width="0.1524" layer="91"/>
<junction x="-16.51" y="99.06"/>
<pinref part="C6" gate="G$1" pin="2"/>
<wire x1="-10.16" y1="85.09" x2="-10.16" y2="96.52" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="96.52" x2="-16.51" y2="96.52" width="0.1524" layer="91"/>
<junction x="-16.51" y="96.52"/>
<junction x="-55.88" y="119.38"/>
<wire x1="-55.88" y1="119.38" x2="-55.88" y2="151.13" width="0.1524" layer="91"/>
<pinref part="R2" gate="G$1" pin="P$2"/>
<wire x1="-45.72" y1="151.13" x2="-52.07" y2="151.13" width="0.1524" layer="91"/>
<wire x1="-52.07" y1="151.13" x2="-52.07" y2="148.59" width="0.1524" layer="91"/>
<wire x1="-55.88" y1="151.13" x2="-52.07" y2="151.13" width="0.1524" layer="91"/>
<junction x="-52.07" y="151.13"/>
<pinref part="TR10" gate="G$1" pin="G"/>
<wire x1="-45.72" y1="142.24" x2="-45.72" y2="151.13" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="USBDM"/>
<wire x1="-21.59" y1="93.98" x2="-21.59" y2="82.55" width="0.1524" layer="91"/>
<pinref part="R4" gate="G$1" pin="P$1"/>
<wire x1="-21.59" y1="82.55" x2="-24.13" y2="82.55" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="USBDP"/>
<pinref part="R1" gate="G$1" pin="P$1"/>
<wire x1="-52.07" y1="93.98" x2="-58.42" y2="93.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="SKT1" gate="G$1" pin="D-VE"/>
<wire x1="-85.09" y1="96.52" x2="-80.01" y2="96.52" width="0.1524" layer="91"/>
<wire x1="-80.01" y1="96.52" x2="-80.01" y2="83.82" width="0.1524" layer="91"/>
<wire x1="-80.01" y1="83.82" x2="-39.37" y2="83.82" width="0.1524" layer="91"/>
<pinref part="C5" gate="G$1" pin="2"/>
<pinref part="R4" gate="G$1" pin="P$2"/>
<wire x1="-34.29" y1="82.55" x2="-39.37" y2="82.55" width="0.1524" layer="91"/>
<wire x1="-39.37" y1="83.82" x2="-39.37" y2="82.55" width="0.1524" layer="91"/>
<junction x="-39.37" y="82.55"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<pinref part="C2" gate="G$1" pin="2"/>
<wire x1="-69.85" y1="81.28" x2="-69.85" y2="93.98" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="P$2"/>
<wire x1="-69.85" y1="93.98" x2="-68.58" y2="93.98" width="0.1524" layer="91"/>
<wire x1="-69.85" y1="93.98" x2="-85.09" y2="93.98" width="0.1524" layer="91"/>
<junction x="-69.85" y="93.98"/>
<pinref part="SKT1" gate="G$1" pin="D+VE"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="C1" gate="G$1" pin="2"/>
<wire x1="-77.47" y1="81.28" x2="-77.47" y2="99.06" width="0.1524" layer="91"/>
<pinref part="L1" gate="G$1" pin="1"/>
<pinref part="SKT1" gate="G$1" pin="+VE"/>
<wire x1="-77.47" y1="99.06" x2="-77.47" y2="105.41" width="0.1524" layer="91"/>
<wire x1="-85.09" y1="99.06" x2="-77.47" y2="99.06" width="0.1524" layer="91"/>
<junction x="-77.47" y="99.06"/>
</segment>
</net>
<net name="N$34" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="!CTS"/>
<wire x1="-52.07" y1="99.06" x2="-53.34" y2="99.06" width="0.1524" layer="91"/>
<wire x1="-53.34" y1="99.06" x2="-53.34" y2="88.9" width="0.1524" layer="91"/>
<wire x1="-53.34" y1="88.9" x2="-2.54" y2="88.9" width="0.1524" layer="91"/>
<pinref part="R6" gate="G$1" pin="P$2"/>
<wire x1="-2.54" y1="82.55" x2="-2.54" y2="88.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$40" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PD0(RXD)"/>
<wire x1="86.36" y1="113.03" x2="88.9" y2="113.03" width="0.1524" layer="91"/>
<wire x1="88.9" y1="113.03" x2="88.9" y2="144.78" width="0.1524" layer="91"/>
<wire x1="88.9" y1="144.78" x2="11.43" y2="144.78" width="0.1524" layer="91"/>
<wire x1="11.43" y1="144.78" x2="11.43" y2="130.81" width="0.1524" layer="91"/>
<wire x1="-35.56" y1="137.16" x2="-13.97" y2="137.16" width="0.1524" layer="91"/>
<wire x1="-13.97" y1="137.16" x2="-13.97" y2="130.81" width="0.1524" layer="91"/>
<wire x1="-13.97" y1="130.81" x2="11.43" y2="130.81" width="0.1524" layer="91"/>
<wire x1="-38.1" y1="137.16" x2="-35.56" y2="137.16" width="0.1524" layer="91"/>
<pinref part="R3" gate="G$1" pin="P$1"/>
<wire x1="-35.56" y1="137.16" x2="-35.56" y2="147.32" width="0.1524" layer="91"/>
<junction x="-35.56" y="137.16"/>
<pinref part="TR10" gate="G$1" pin="D"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="TXD"/>
<wire x1="-52.07" y1="111.76" x2="-52.07" y2="137.16" width="0.1524" layer="91"/>
<pinref part="R2" gate="G$1" pin="P$1"/>
<pinref part="TR10" gate="G$1" pin="S"/>
<wire x1="-52.07" y1="137.16" x2="-52.07" y2="138.43" width="0.1524" layer="91"/>
<wire x1="-48.26" y1="137.16" x2="-52.07" y2="137.16" width="0.1524" layer="91"/>
<junction x="-52.07" y="137.16"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="TR8" gate="G$1" pin="D"/>
<wire x1="149.86" y1="143.51" x2="161.29" y2="143.51" width="0.1524" layer="91"/>
<wire x1="161.29" y1="143.51" x2="161.29" y2="205.74" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="DIG2"/>
<wire x1="161.29" y1="205.74" x2="199.39" y2="205.74" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="TR5" gate="G$1" pin="D"/>
<wire x1="130.81" y1="138.43" x2="162.56" y2="138.43" width="0.1524" layer="91"/>
<wire x1="162.56" y1="138.43" x2="162.56" y2="203.2" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="DIG3"/>
<wire x1="162.56" y1="203.2" x2="199.39" y2="203.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="TR9" gate="G$1" pin="D"/>
<wire x1="149.86" y1="127" x2="163.83" y2="127" width="0.1524" layer="91"/>
<wire x1="163.83" y1="127" x2="163.83" y2="198.12" width="0.1524" layer="91"/>
<pinref part="7_SEG1" gate="G$1" pin="DIG4"/>
<wire x1="163.83" y1="198.12" x2="199.39" y2="198.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="IC3" gate="G$1" pin="PC2(ADC2)"/>
<wire x1="86.36" y1="130.81" x2="144.78" y2="130.81" width="0.1524" layer="91"/>
<pinref part="TR9" gate="G$1" pin="G"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
<errors>
<approved hash="104,1,38.1,105.41,IC3,GND,0V,,,"/>
<approved hash="104,1,38.1,100.33,IC3,VCC,+5V,,,"/>
<approved hash="104,1,38.1,130.81,IC3,GND,0V,,,"/>
<approved hash="104,1,38.1,125.73,IC3,AVCC,+5V,,,"/>
</errors>
</schematic>
</drawing>
<compatibility>
<note version="6.3" minversion="6.2.2" severity="warning">
Since Version 6.2.2 text objects can contain more than one line,
which will not be processed correctly with this version.
</note>
</compatibility>
</eagle>
