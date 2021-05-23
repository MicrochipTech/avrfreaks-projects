<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.5.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.05" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="24" fill="1" visible="no" active="no"/>
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
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
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
<library name="1_home_library">
<packages>
<package name="6_WAY_HEADER">
<pad name="P$1" x="0" y="0" drill="1.143" shape="square"/>
<pad name="P$2" x="0" y="-2.54" drill="1.143" shape="square"/>
<pad name="P$3" x="0" y="-5.08" drill="1.143" shape="square"/>
<pad name="P$4" x="0" y="-7.62" drill="1.143" shape="square"/>
<pad name="P$5" x="0" y="-10.16" drill="1.143" shape="square"/>
<pad name="P$6" x="0" y="-12.7" drill="1.143" shape="square"/>
</package>
<package name="8_WAY_HEADER">
<pad name="P$1" x="0" y="0" drill="1.143" shape="square"/>
<pad name="P$2" x="0" y="-2.54" drill="1.143" shape="square"/>
<pad name="P$3" x="0" y="-5.08" drill="1.143" shape="square"/>
<pad name="P$4" x="0" y="-7.62" drill="1.143" shape="square"/>
<pad name="P$5" x="0" y="-10.16" drill="1.143" shape="square"/>
<pad name="P$6" x="0" y="-12.7" drill="1.143" shape="square"/>
<pad name="P$7" x="0" y="-15.24" drill="1.143" shape="square"/>
<pad name="P$8" x="0" y="-17.78" drill="1.143" shape="square"/>
</package>
<package name="1206">
<smd name="P$1" x="0" y="0" dx="2.2" dy="1.7" layer="1" rot="R180"/>
<smd name="P$2" x="3.61" y="0" dx="2.2" dy="1.7" layer="1" rot="R180"/>
</package>
<package name="DISPLAY">
<pad name="P$1" x="0" y="0" drill="1" shape="square"/>
<pad name="P$2" x="2.54" y="0" drill="1" shape="square"/>
<pad name="P$3" x="5.08" y="0" drill="1" shape="square"/>
<pad name="P$4" x="7.62" y="0" drill="1" shape="square"/>
<pad name="P$5" x="10.16" y="0" drill="1" shape="square"/>
<pad name="P$6" x="12.7" y="0" drill="1" shape="square"/>
<pad name="P$7" x="12.7" y="15.24" drill="1" shape="square"/>
<pad name="P$8" x="10.16" y="15.24" drill="1" shape="square"/>
<pad name="P$9" x="7.62" y="15.24" drill="1" shape="square"/>
<pad name="P$10" x="5.08" y="15.24" drill="1" shape="square"/>
<pad name="P$11" x="2.54" y="15.24" drill="1" shape="square"/>
<pad name="P$12" x="0" y="15.24" drill="1" shape="square"/>
<wire x1="31.496" y1="17.272" x2="-18.796" y2="17.272" width="0.127" layer="21"/>
<wire x1="-18.796" y1="-1.905" x2="31.496" y2="-1.905" width="0.127" layer="21"/>
<wire x1="31.496" y1="-1.905" x2="31.496" y2="17.272" width="0.127" layer="21"/>
<wire x1="-18.796" y1="17.272" x2="-18.796" y2="-1.905" width="0.127" layer="21"/>
</package>
<package name="PUSH_BUTTON_NO_SM">
<smd name="P$1" x="-0.03" y="-0.78" dx="3" dy="4.5" layer="1" rot="R180"/>
<smd name="P$2" x="-0.03" y="8.42" dx="3" dy="4.5" layer="1" rot="R180"/>
</package>
<package name="PB_NO_TH">
<smd name="P$1" x="0" y="0" dx="2" dy="4" layer="1"/>
<smd name="P$2" x="6.75" y="0" dx="2" dy="4" layer="1"/>
</package>
</packages>
<symbols>
<symbol name="6_WAY_HEADER">
<pin name="1" x="0" y="0" visible="pin" length="short"/>
<pin name="2" x="0" y="-2.54" visible="pin" length="short"/>
<pin name="3" x="0" y="-5.08" visible="pin" length="short"/>
<pin name="4" x="0" y="-7.62" visible="pin" length="short"/>
<pin name="5" x="0" y="-10.16" visible="pin" length="short"/>
<pin name="6" x="0" y="-12.7" visible="pin" length="short"/>
<circle x="2.54" y="0" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-2.54" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-5.08" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-7.62" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-10.16" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-12.7" radius="0.635" width="0.254" layer="94"/>
<text x="0" y="-15.24" size="1.778" layer="94">&gt;NAME</text>
</symbol>
<symbol name="8_WAY_HEADER">
<pin name="1" x="0" y="0" visible="pin" length="short"/>
<pin name="2" x="0" y="-2.54" visible="pin" length="short"/>
<pin name="3" x="0" y="-5.08" visible="pin" length="short"/>
<pin name="4" x="0" y="-7.62" visible="pin" length="short"/>
<pin name="5" x="0" y="-10.16" visible="pin" length="short"/>
<pin name="6" x="0" y="-12.7" visible="pin" length="short"/>
<pin name="7" x="0" y="-15.24" visible="pin" length="short"/>
<pin name="8" x="0" y="-17.78" visible="pin" length="short"/>
<circle x="2.54" y="0" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-2.54" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-5.08" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-7.62" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-10.16" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-12.7" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-15.24" radius="0.635" width="0.254" layer="94"/>
<circle x="2.54" y="-17.78" radius="0.635" width="0.254" layer="94"/>
<text x="2.54" y="-20.32" size="1.778" layer="94">&gt;NAME</text>
</symbol>
<symbol name="RESISTOR">
<pin name="1" x="0" y="0" visible="off" length="short"/>
<pin name="2" x="8.89" y="0" visible="off" length="short" rot="R180"/>
<wire x1="2.54" y1="0.635" x2="6.35" y2="0.635" width="0.254" layer="94"/>
<wire x1="6.35" y1="0.635" x2="6.35" y2="-0.635" width="0.254" layer="94"/>
<wire x1="6.35" y1="-0.635" x2="2.54" y2="-0.635" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.635" x2="2.54" y2="0.635" width="0.254" layer="94"/>
<text x="3.302" y="1.524" size="1.778" layer="94">&gt;NAME</text>
<text x="2.54" y="-2.54" size="1.778" layer="94">&gt;VALUE</text>
</symbol>
<symbol name="DISPLAY">
<pin name="E" x="0" y="-10.16" visible="pin" length="short"/>
<pin name="D" x="0" y="-7.62" visible="pin" length="short"/>
<pin name="CLON" x="0" y="-5.08" visible="pin" length="short"/>
<pin name="C" x="0" y="-2.54" visible="pin" length="short"/>
<pin name="G" x="0" y="0" visible="pin" length="short"/>
<pin name="DIG4" x="0" y="2.54" visible="pin" length="short"/>
<pin name="B" x="0" y="5.08" visible="pin" length="short"/>
<pin name="DIG3" x="0" y="7.62" visible="pin" length="short"/>
<pin name="DIG2" x="0" y="10.16" visible="pin" length="short"/>
<pin name="F" x="0" y="12.7" visible="pin" length="short"/>
<pin name="A" x="0" y="15.24" visible="pin" length="short"/>
<pin name="DIG1" x="0" y="17.78" visible="pin" length="short"/>
<wire x1="2.54" y1="20.32" x2="2.54" y2="-12.7" width="0.254" layer="94"/>
<wire x1="2.54" y1="-12.7" x2="12.7" y2="-12.7" width="0.254" layer="94"/>
<wire x1="12.7" y1="-12.7" x2="12.7" y2="20.32" width="0.254" layer="94"/>
<wire x1="12.7" y1="20.32" x2="2.54" y2="20.32" width="0.254" layer="94"/>
</symbol>
<symbol name="PUSH_BUTTON_NO_SM">
<pin name="P$1" x="0" y="0" visible="off" length="point"/>
<pin name="P$2" x="5.08" y="0" visible="off" length="point"/>
<circle x="0.127" y="0" radius="0.924571875" width="0.254" layer="94"/>
<circle x="5.08" y="0" radius="0.851940625" width="0.254" layer="94"/>
<wire x1="5.08" y1="-0.127" x2="-0.254" y2="1.651" width="0.254" layer="94"/>
<text x="2.54" y="2.54" size="1.778" layer="94">&gt;NAME</text>
</symbol>
<symbol name="CAPACITOR">
<pin name="P$1" x="0" y="0" visible="off" length="short"/>
<pin name="P$2" x="7.62" y="0" visible="off" length="short" rot="R180"/>
<wire x1="3.175" y1="2.54" x2="3.175" y2="0" width="0.254" layer="94"/>
<wire x1="3.175" y1="0" x2="3.175" y2="-2.54" width="0.254" layer="94"/>
<wire x1="4.445" y1="2.54" x2="4.445" y2="0" width="0.254" layer="94"/>
<wire x1="4.445" y1="0" x2="4.445" y2="-2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="0" x2="3.175" y2="0" width="0.254" layer="94"/>
<wire x1="5.08" y1="0" x2="4.445" y2="0" width="0.254" layer="94"/>
<text x="5.08" y="2.54" size="1.778" layer="94">&gt;NAME</text>
<text x="5.08" y="-2.54" size="1.778" layer="94">&gt;VALUE</text>
</symbol>
<symbol name="PB_TH_NO">
<pin name="P$1" x="0" y="0" visible="off" length="point"/>
<pin name="P$2" x="5.08" y="0" visible="off" length="point"/>
<wire x1="5.08" y1="0" x2="-0.254" y2="1.778" width="0.254" layer="94"/>
<circle x="0" y="0" radius="0.915809375" width="0.254" layer="94"/>
<circle x="5.08" y="0" radius="0.915809375" width="0.254" layer="94"/>
<text x="5.08" y="2.54" size="1.778" layer="94">&gt;NAME</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="6_WAY_HEADER" prefix="SK">
<gates>
<gate name="G$1" symbol="6_WAY_HEADER" x="50.8" y="5.08"/>
</gates>
<devices>
<device name="" package="6_WAY_HEADER">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
<connect gate="G$1" pin="3" pad="P$3"/>
<connect gate="G$1" pin="4" pad="P$4"/>
<connect gate="G$1" pin="5" pad="P$5"/>
<connect gate="G$1" pin="6" pad="P$6"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="8_WAY_HEADER" prefix="SK">
<gates>
<gate name="G$1" symbol="8_WAY_HEADER" x="-10.16" y="40.64"/>
</gates>
<devices>
<device name="" package="8_WAY_HEADER">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="2" pad="P$2"/>
<connect gate="G$1" pin="3" pad="P$3"/>
<connect gate="G$1" pin="4" pad="P$4"/>
<connect gate="G$1" pin="5" pad="P$5"/>
<connect gate="G$1" pin="6" pad="P$6"/>
<connect gate="G$1" pin="7" pad="P$7"/>
<connect gate="G$1" pin="8" pad="P$8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="RESISTOR" prefix="R" uservalue="yes">
<gates>
<gate name="G$1" symbol="RESISTOR" x="7.62" y="0"/>
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
<deviceset name="DISPLAY">
<gates>
<gate name="G$1" symbol="DISPLAY" x="-76.2" y="15.24"/>
</gates>
<devices>
<device name="" package="DISPLAY">
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
<deviceset name="PUSH_BUTTON_NO_SM" prefix="SW">
<gates>
<gate name="G$1" symbol="PUSH_BUTTON_NO_SM" x="-2.54" y="12.7"/>
</gates>
<devices>
<device name="" package="PUSH_BUTTON_NO_SM">
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
<deviceset name="CAPACITOR" prefix="C" uservalue="yes">
<gates>
<gate name="G$1" symbol="CAPACITOR" x="-5.08" y="12.7"/>
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
<deviceset name="BP_TH_NO" prefix="SW">
<gates>
<gate name="G$1" symbol="PB_TH_NO" x="-5.08" y="15.24"/>
</gates>
<devices>
<device name="" package="PB_NO_TH">
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
</devicesets>
</library>
<library name="supply1" urn="urn:adsk.eagle:library:371">
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
<symbol name="+5V" urn="urn:adsk.eagle:symbol:26929/1" library_version="1">
<wire x1="1.27" y1="-1.905" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="-1.27" y2="-1.905" width="0.254" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="+5V" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="0V" urn="urn:adsk.eagle:symbol:26927/1" library_version="1">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-1.905" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="0V" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="+5V" urn="urn:adsk.eagle:component:26963/1" prefix="P+" library_version="1">
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
<deviceset name="0V" urn="urn:adsk.eagle:component:26956/1" prefix="GND" library_version="1">
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
<library name="1_home_library_2">
<packages>
<package name="DIL20" urn="urn:adsk.eagle:footprint:4168/1">
<description>&lt;B&gt;Dual In Line&lt;/B&gt;</description>
<wire x1="-13.208" y1="-0.635" x2="-13.208" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="-13.208" y1="-0.635" x2="-13.208" y2="0.635" width="0.1524" layer="21" curve="180"/>
<wire x1="13.208" y1="-2.794" x2="13.208" y2="2.794" width="0.1524" layer="21"/>
<wire x1="13.208" y1="-2.794" x2="-13.208" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="-13.208" y1="2.794" x2="-13.208" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-13.208" y1="2.794" x2="13.208" y2="2.794" width="0.1524" layer="21"/>
<pad name="1" x="-11.43" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-8.89" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-6.35" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="6.35" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="8.89" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="11.43" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="11.43" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="8.89" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="6.35" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="-1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="-3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="-6.35" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="-8.89" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="-11.43" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-13.5128" y="-2.54" size="1.778" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-10.16" y="-0.762" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="TINY861">
<wire x1="-7.62" y1="20.32" x2="15.24" y2="20.32" width="0.254" layer="94"/>
<wire x1="15.24" y1="20.32" x2="15.24" y2="-15.24" width="0.254" layer="94"/>
<wire x1="15.24" y1="-15.24" x2="-7.62" y2="-15.24" width="0.254" layer="94"/>
<wire x1="-7.62" y1="-15.24" x2="-7.62" y2="20.32" width="0.254" layer="94"/>
<text x="-10.16" y="-17.78" size="1.778" layer="96">&gt;VALUE</text>
<text x="-10.16" y="21.082" size="1.778" layer="95">&gt;NAME</text>
<pin name="PB5" x="17.78" y="15.24" visible="pin" length="short" rot="R180"/>
<pin name="PB4" x="17.78" y="12.7" visible="pin" length="short" rot="R180"/>
<pin name="PB3" x="17.78" y="10.16" visible="pin" length="short" rot="R180"/>
<pin name="SCK_PB2" x="17.78" y="2.54" visible="pin" length="short" rot="R180"/>
<pin name="MISO_PB1" x="17.78" y="5.08" visible="pin" length="short" rot="R180"/>
<pin name="MOSI_PB0" x="17.78" y="7.62" visible="pin" length="short" rot="R180"/>
<pin name="PA7" x="17.78" y="-2.54" visible="pin" length="short" rot="R180"/>
<pin name="PA6" x="17.78" y="-5.08" visible="pin" length="short" rot="R180"/>
<pin name="PA5" x="17.78" y="-7.62" visible="pin" length="short" rot="R180"/>
<pin name="PA4" x="17.78" y="-10.16" visible="pin" length="short" rot="R180"/>
<pin name="PA3" x="17.78" y="-12.7" visible="pin" length="short" rot="R180"/>
<pin name="USCK_PA2" x="-10.16" y="-5.08" visible="pin" length="short"/>
<pin name="DO_PA1" x="-10.16" y="-7.62" visible="pin" length="short"/>
<pin name="DI_PCINT0_PA0" x="-10.16" y="0" visible="pin" length="short"/>
<pin name="GND" x="-10.16" y="12.7" visible="pin" length="short" direction="pwr"/>
<pin name="VCC" x="-10.16" y="17.78" visible="pin" length="short" direction="pwr"/>
<pin name="AVCC" x="-10.16" y="15.24" visible="pin" length="short" direction="pwr"/>
<pin name="RESET_PB7" x="-10.16" y="-2.54" visible="pin" length="short"/>
<pin name="PB6" x="17.78" y="17.78" visible="pin" length="short" rot="R180"/>
<pin name="GND@1" x="-10.16" y="10.16" visible="pin" length="short" direction="pwr"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="ATTINY861">
<gates>
<gate name="G$1" symbol="TINY861" x="-10.16" y="-5.08"/>
</gates>
<devices>
<device name="" package="DIL20">
<connects>
<connect gate="G$1" pin="AVCC" pad="15"/>
<connect gate="G$1" pin="DI_PCINT0_PA0" pad="20"/>
<connect gate="G$1" pin="DO_PA1" pad="19"/>
<connect gate="G$1" pin="GND" pad="6"/>
<connect gate="G$1" pin="GND@1" pad="16"/>
<connect gate="G$1" pin="MISO_PB1" pad="2"/>
<connect gate="G$1" pin="MOSI_PB0" pad="1"/>
<connect gate="G$1" pin="PA3" pad="17"/>
<connect gate="G$1" pin="PA4" pad="14"/>
<connect gate="G$1" pin="PA5" pad="13"/>
<connect gate="G$1" pin="PA6" pad="12"/>
<connect gate="G$1" pin="PA7" pad="11"/>
<connect gate="G$1" pin="PB3" pad="4"/>
<connect gate="G$1" pin="PB4" pad="7"/>
<connect gate="G$1" pin="PB5" pad="8"/>
<connect gate="G$1" pin="PB6" pad="9"/>
<connect gate="G$1" pin="RESET_PB7" pad="10"/>
<connect gate="G$1" pin="SCK_PB2" pad="3"/>
<connect gate="G$1" pin="USCK_PA2" pad="18"/>
<connect gate="G$1" pin="VCC" pad="5"/>
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
<part name="SK3" library="1_home_library" deviceset="6_WAY_HEADER" device=""/>
<part name="SK1" library="1_home_library" deviceset="8_WAY_HEADER" device=""/>
<part name="SK2" library="1_home_library" deviceset="6_WAY_HEADER" device=""/>
<part name="SK4" library="1_home_library" deviceset="6_WAY_HEADER" device=""/>
<part name="U$3" library="1_home_library" deviceset="DISPLAY" device=""/>
<part name="U$4" library="1_home_library" deviceset="DISPLAY" device=""/>
<part name="GND3" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="P+3" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="SW4" library="1_home_library" deviceset="PUSH_BUTTON_NO_SM" device=""/>
<part name="R10" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R11" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R12" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="C5" library="1_home_library" deviceset="CAPACITOR" device="" value="10n"/>
<part name="C6" library="1_home_library" deviceset="CAPACITOR" device="" value="10n"/>
<part name="C7" library="1_home_library" deviceset="CAPACITOR" device="" value="10n"/>
<part name="SW3" library="1_home_library" deviceset="BP_TH_NO" device=""/>
<part name="SW2" library="1_home_library" deviceset="BP_TH_NO" device=""/>
<part name="SW1" library="1_home_library" deviceset="BP_TH_NO" device=""/>
<part name="GND4" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="R14" library="1_home_library" deviceset="RESISTOR" device="" value="3K9"/>
<part name="R15" library="1_home_library" deviceset="RESISTOR" device="" value="3K9"/>
<part name="R16" library="1_home_library" deviceset="RESISTOR" device="" value="100R"/>
<part name="C1" library="1_home_library" deviceset="CAPACITOR" device="" value="10nF"/>
<part name="GND6" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="P+5" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="C2" library="1_home_library" deviceset="CAPACITOR" device="" value="4u7"/>
<part name="C3" library="1_home_library" deviceset="CAPACITOR" device="" value="100n"/>
<part name="C4" library="1_home_library" deviceset="CAPACITOR" device="" value="100n"/>
<part name="P+6" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="GND7" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="GND8" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="P+7" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="U$1" library="1_home_library_2" deviceset="ATTINY861" device=""/>
<part name="U$2" library="1_home_library_2" deviceset="ATTINY861" device=""/>
<part name="GND1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="GND2" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="P+1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="P+2" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="GND5" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="R2" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R1" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R3" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R4" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R5" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R6" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R7" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R8" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R9" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R13" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R17" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R18" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R19" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R20" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R21" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="R22" library="1_home_library" deviceset="RESISTOR" device="" value="220R"/>
<part name="C8" library="1_home_library" deviceset="CAPACITOR" device="" value="100n"/>
<part name="C9" library="1_home_library" deviceset="CAPACITOR" device="" value="4u7"/>
<part name="C10" library="1_home_library" deviceset="CAPACITOR" device="" value="4u7"/>
<part name="R23" library="1_home_library" deviceset="RESISTOR" device="" value="10K"/>
<part name="P+4" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="SK3" gate="G$1" x="-175.26" y="137.16" smashed="yes" rot="R270">
<attribute name="NAME" x="-187.96" y="139.7" size="1.778" layer="94"/>
</instance>
<instance part="SK1" gate="G$1" x="-228.6" y="213.36" smashed="yes" rot="MR90">
<attribute name="NAME" x="-228.6" y="220.98" size="1.778" layer="94" rot="MR0"/>
</instance>
<instance part="SK2" gate="G$1" x="-299.72" y="223.52" smashed="yes" rot="MR90">
<attribute name="NAME" x="-299.72" y="231.14" size="1.778" layer="94" rot="MR0"/>
</instance>
<instance part="SK4" gate="G$1" x="-223.52" y="137.16" smashed="yes" rot="R270">
<attribute name="NAME" x="-228.6" y="127" size="1.778" layer="94"/>
</instance>
<instance part="U$3" gate="G$1" x="-69.85" y="223.52" smashed="yes"/>
<instance part="U$4" gate="G$1" x="-67.31" y="175.26" smashed="yes"/>
<instance part="GND3" gate="1" x="-210.82" y="139.7" smashed="yes">
<attribute name="VALUE" x="-212.725" y="137.16" size="1.778" layer="96"/>
</instance>
<instance part="P+3" gate="1" x="-231.14" y="149.86" smashed="yes">
<attribute name="VALUE" x="-233.68" y="144.78" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="SW4" gate="G$1" x="-121.92" y="146.05" smashed="yes" rot="R270">
<attribute name="NAME" x="-119.38" y="143.51" size="1.778" layer="94"/>
</instance>
<instance part="R10" gate="G$1" x="-236.22" y="264.16" smashed="yes">
<attribute name="NAME" x="-232.918" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-233.68" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="R11" gate="G$1" x="-215.9" y="264.16" smashed="yes">
<attribute name="NAME" x="-212.598" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-213.36" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="R12" gate="G$1" x="-190.5" y="264.16" smashed="yes">
<attribute name="NAME" x="-187.198" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-187.96" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="C5" gate="G$1" x="-238.76" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-241.3" y="259.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-241.3" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C6" gate="G$1" x="-218.44" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-219.71" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-213.36" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C7" gate="G$1" x="-193.04" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-194.31" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-187.96" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="SW3" gate="G$1" x="-177.8" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-185.42" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="SW2" gate="G$1" x="-203.2" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-210.82" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="SW1" gate="G$1" x="-226.06" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-233.68" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="GND4" gate="1" x="-177.8" y="243.84" smashed="yes">
<attribute name="VALUE" x="-179.705" y="241.3" size="1.778" layer="96"/>
</instance>
<instance part="R14" gate="G$1" x="-165.1" y="142.24" smashed="yes">
<attribute name="NAME" x="-161.798" y="143.764" size="1.778" layer="94"/>
<attribute name="VALUE" x="-156.21" y="143.51" size="1.778" layer="94"/>
</instance>
<instance part="R15" gate="G$1" x="-165.1" y="137.16" smashed="yes">
<attribute name="NAME" x="-161.798" y="138.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-162.56" y="134.62" size="1.778" layer="94"/>
</instance>
<instance part="R16" gate="G$1" x="-149.86" y="139.7" smashed="yes">
<attribute name="NAME" x="-146.558" y="141.224" size="1.778" layer="94"/>
<attribute name="VALUE" x="-147.32" y="137.16" size="1.778" layer="94"/>
</instance>
<instance part="C1" gate="G$1" x="-152.4" y="129.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-153.67" y="132.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-146.05" y="132.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="GND6" gate="1" x="-152.4" y="124.46" smashed="yes">
<attribute name="VALUE" x="-154.305" y="121.92" size="1.778" layer="96"/>
</instance>
<instance part="P+5" gate="1" x="-134.62" y="139.7" smashed="yes" rot="R270">
<attribute name="VALUE" x="-139.7" y="142.24" size="1.778" layer="96"/>
</instance>
<instance part="C2" gate="G$1" x="-304.8" y="256.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-309.88" y="259.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-304.8" y="259.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C3" gate="G$1" x="-292.1" y="256.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-293.37" y="262.89" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-290.83" y="259.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C4" gate="G$1" x="-251.46" y="132.08" smashed="yes" rot="R90">
<attribute name="NAME" x="-255.27" y="137.16" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-255.27" y="134.62" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="P+6" gate="1" x="-304.8" y="271.78" smashed="yes">
<attribute name="VALUE" x="-307.34" y="266.7" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="GND7" gate="1" x="-304.8" y="243.84" smashed="yes">
<attribute name="VALUE" x="-306.705" y="241.3" size="1.778" layer="96"/>
</instance>
<instance part="GND8" gate="1" x="-251.46" y="129.54" smashed="yes">
<attribute name="VALUE" x="-253.365" y="127" size="1.778" layer="96"/>
</instance>
<instance part="P+7" gate="1" x="-251.46" y="147.32" smashed="yes">
<attribute name="VALUE" x="-254" y="142.24" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="U$1" gate="G$1" x="-127" y="223.52" smashed="yes">
<attribute name="VALUE" x="-137.16" y="205.74" size="1.778" layer="96"/>
<attribute name="NAME" x="-137.16" y="244.602" size="1.778" layer="95"/>
</instance>
<instance part="U$2" gate="G$1" x="-127" y="175.26" smashed="yes">
<attribute name="VALUE" x="-137.16" y="157.48" size="1.778" layer="96"/>
<attribute name="NAME" x="-137.16" y="196.342" size="1.778" layer="95"/>
</instance>
<instance part="GND1" gate="1" x="-139.7" y="229.87" smashed="yes">
<attribute name="VALUE" x="-141.605" y="227.33" size="1.778" layer="96"/>
</instance>
<instance part="GND2" gate="1" x="-139.7" y="181.61" smashed="yes">
<attribute name="VALUE" x="-141.605" y="179.07" size="1.778" layer="96"/>
</instance>
<instance part="P+1" gate="1" x="-139.7" y="246.38" smashed="yes">
<attribute name="VALUE" x="-142.24" y="241.3" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="P+2" gate="1" x="-139.7" y="196.85" smashed="yes">
<attribute name="VALUE" x="-142.24" y="191.77" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="GND5" gate="1" x="-121.92" y="130.81" smashed="yes">
<attribute name="VALUE" x="-123.825" y="128.27" size="1.778" layer="96"/>
</instance>
<instance part="R2" gate="G$1" x="-88.9" y="238.76" smashed="yes">
<attribute name="NAME" x="-85.598" y="240.284" size="1.778" layer="94"/>
<attribute name="VALUE" x="-86.36" y="236.22" size="1.778" layer="94"/>
</instance>
<instance part="R1" gate="G$1" x="-81.28" y="236.22" smashed="yes">
<attribute name="NAME" x="-77.978" y="237.744" size="1.778" layer="94"/>
<attribute name="VALUE" x="-78.74" y="233.68" size="1.778" layer="94"/>
</instance>
<instance part="R3" gate="G$1" x="-80.01" y="228.6" smashed="yes">
<attribute name="NAME" x="-76.708" y="230.124" size="1.778" layer="94"/>
<attribute name="VALUE" x="-77.47" y="226.06" size="1.778" layer="94"/>
</instance>
<instance part="R4" gate="G$1" x="-90.17" y="223.52" smashed="yes">
<attribute name="NAME" x="-86.868" y="225.044" size="1.778" layer="94"/>
<attribute name="VALUE" x="-87.63" y="220.98" size="1.778" layer="94"/>
</instance>
<instance part="R5" gate="G$1" x="-80.01" y="215.9" smashed="yes">
<attribute name="NAME" x="-76.708" y="217.424" size="1.778" layer="94"/>
<attribute name="VALUE" x="-77.47" y="213.36" size="1.778" layer="94"/>
</instance>
<instance part="R6" gate="G$1" x="-88.9" y="213.36" smashed="yes">
<attribute name="NAME" x="-85.598" y="214.884" size="1.778" layer="94"/>
<attribute name="VALUE" x="-86.36" y="210.82" size="1.778" layer="94"/>
</instance>
<instance part="R7" gate="G$1" x="-80.01" y="220.98" smashed="yes">
<attribute name="NAME" x="-76.708" y="222.504" size="1.778" layer="94"/>
<attribute name="VALUE" x="-77.47" y="218.44" size="1.778" layer="94"/>
</instance>
<instance part="R8" gate="G$1" x="-88.9" y="218.44" smashed="yes">
<attribute name="NAME" x="-85.598" y="219.964" size="1.778" layer="94"/>
<attribute name="VALUE" x="-86.36" y="215.9" size="1.778" layer="94"/>
</instance>
<instance part="R9" gate="G$1" x="-80.01" y="190.5" smashed="yes">
<attribute name="NAME" x="-76.708" y="192.024" size="1.778" layer="94"/>
<attribute name="VALUE" x="-77.47" y="187.96" size="1.778" layer="94"/>
</instance>
<instance part="R13" gate="G$1" x="-92.71" y="187.96" smashed="yes">
<attribute name="NAME" x="-89.408" y="189.484" size="1.778" layer="94"/>
<attribute name="VALUE" x="-90.17" y="185.42" size="1.778" layer="94"/>
</instance>
<instance part="R17" gate="G$1" x="-96.52" y="182.88" smashed="yes">
<attribute name="NAME" x="-93.218" y="184.404" size="1.778" layer="94"/>
<attribute name="VALUE" x="-93.98" y="180.34" size="1.778" layer="94"/>
</instance>
<instance part="R18" gate="G$1" x="-82.55" y="175.26" smashed="yes">
<attribute name="NAME" x="-79.248" y="176.784" size="1.778" layer="94"/>
<attribute name="VALUE" x="-80.01" y="172.72" size="1.778" layer="94"/>
</instance>
<instance part="R19" gate="G$1" x="-93.98" y="172.72" smashed="yes">
<attribute name="NAME" x="-90.678" y="174.244" size="1.778" layer="94"/>
<attribute name="VALUE" x="-91.44" y="170.18" size="1.778" layer="94"/>
</instance>
<instance part="R20" gate="G$1" x="-78.74" y="167.64" smashed="yes">
<attribute name="NAME" x="-75.438" y="169.164" size="1.778" layer="94"/>
<attribute name="VALUE" x="-76.2" y="165.1" size="1.778" layer="94"/>
</instance>
<instance part="R21" gate="G$1" x="-86.36" y="165.1" smashed="yes">
<attribute name="NAME" x="-83.058" y="166.624" size="1.778" layer="94"/>
<attribute name="VALUE" x="-83.82" y="162.56" size="1.778" layer="94"/>
</instance>
<instance part="R22" gate="G$1" x="-101.6" y="170.18" smashed="yes">
<attribute name="NAME" x="-98.298" y="171.704" size="1.778" layer="94"/>
<attribute name="VALUE" x="-99.06" y="167.64" size="1.778" layer="94"/>
</instance>
<instance part="C8" gate="G$1" x="-276.86" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-278.13" y="260.35" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-275.59" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C9" gate="G$1" x="-320.04" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-325.12" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-320.04" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C10" gate="G$1" x="-335.28" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-340.36" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-335.28" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R23" gate="G$1" x="-194.31" y="156.21" smashed="yes">
<attribute name="NAME" x="-191.008" y="157.734" size="1.778" layer="94"/>
<attribute name="VALUE" x="-191.77" y="153.67" size="1.778" layer="94"/>
</instance>
<instance part="P+4" gate="1" x="-198.12" y="160.02" smashed="yes">
<attribute name="VALUE" x="-200.66" y="154.94" size="1.778" layer="96" rot="R90"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="0V" class="0">
<segment>
<pinref part="SK4" gate="G$1" pin="2"/>
<wire x1="-226.06" y1="137.16" x2="-226.06" y2="139.7" width="0.1524" layer="91"/>
<wire x1="-226.06" y1="139.7" x2="-226.06" y2="144.78" width="0.1524" layer="91"/>
<wire x1="-226.06" y1="144.78" x2="-210.82" y2="144.78" width="0.1524" layer="91"/>
<pinref part="GND3" gate="1" pin="0V"/>
<wire x1="-210.82" y1="144.78" x2="-210.82" y2="142.24" width="0.1524" layer="91"/>
<pinref part="SK4" gate="G$1" pin="3"/>
<wire x1="-228.6" y1="137.16" x2="-228.6" y2="139.7" width="0.1524" layer="91"/>
<wire x1="-228.6" y1="139.7" x2="-226.06" y2="139.7" width="0.1524" layer="91"/>
<junction x="-226.06" y="139.7"/>
</segment>
<segment>
<pinref part="C5" gate="G$1" pin="P$1"/>
<wire x1="-238.76" y1="254" x2="-238.76" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-238.76" y1="248.92" x2="-226.06" y2="248.92" width="0.1524" layer="91"/>
<pinref part="SW3" gate="G$1" pin="P$2"/>
<wire x1="-226.06" y1="248.92" x2="-218.44" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-218.44" y1="248.92" x2="-203.2" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-203.2" y1="248.92" x2="-193.04" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-193.04" y1="248.92" x2="-177.8" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-177.8" y1="248.92" x2="-177.8" y2="254" width="0.1524" layer="91"/>
<pinref part="C7" gate="G$1" pin="P$1"/>
<wire x1="-193.04" y1="254" x2="-193.04" y2="248.92" width="0.1524" layer="91"/>
<junction x="-193.04" y="248.92"/>
<pinref part="SW2" gate="G$1" pin="P$2"/>
<wire x1="-203.2" y1="254" x2="-203.2" y2="248.92" width="0.1524" layer="91"/>
<junction x="-203.2" y="248.92"/>
<pinref part="C6" gate="G$1" pin="P$1"/>
<wire x1="-218.44" y1="254" x2="-218.44" y2="248.92" width="0.1524" layer="91"/>
<junction x="-218.44" y="248.92"/>
<pinref part="SW1" gate="G$1" pin="P$2"/>
<wire x1="-226.06" y1="254" x2="-226.06" y2="248.92" width="0.1524" layer="91"/>
<junction x="-226.06" y="248.92"/>
<pinref part="GND4" gate="1" pin="0V"/>
<wire x1="-177.8" y1="246.38" x2="-177.8" y2="248.92" width="0.1524" layer="91"/>
<junction x="-177.8" y="248.92"/>
</segment>
<segment>
<pinref part="GND6" gate="1" pin="0V"/>
<pinref part="C1" gate="G$1" pin="P$1"/>
<wire x1="-152.4" y1="127" x2="-152.4" y2="129.54" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="P$1"/>
<pinref part="GND7" gate="1" pin="0V"/>
<wire x1="-304.8" y1="246.38" x2="-304.8" y2="251.46" width="0.1524" layer="91"/>
<pinref part="C3" gate="G$1" pin="P$1"/>
<wire x1="-304.8" y1="251.46" x2="-304.8" y2="256.54" width="0.1524" layer="91"/>
<wire x1="-292.1" y1="256.54" x2="-292.1" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-292.1" y1="251.46" x2="-304.8" y2="251.46" width="0.1524" layer="91"/>
<junction x="-304.8" y="251.46"/>
<pinref part="C8" gate="G$1" pin="P$1"/>
<wire x1="-276.86" y1="254" x2="-276.86" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-276.86" y1="251.46" x2="-292.1" y2="251.46" width="0.1524" layer="91"/>
<junction x="-292.1" y="251.46"/>
<wire x1="-304.8" y1="251.46" x2="-320.04" y2="251.46" width="0.1524" layer="91"/>
<pinref part="C9" gate="G$1" pin="P$1"/>
<wire x1="-320.04" y1="251.46" x2="-320.04" y2="254" width="0.1524" layer="91"/>
<pinref part="C10" gate="G$1" pin="P$1"/>
<wire x1="-320.04" y1="251.46" x2="-335.28" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-335.28" y1="251.46" x2="-335.28" y2="254" width="0.1524" layer="91"/>
<junction x="-320.04" y="251.46"/>
</segment>
<segment>
<pinref part="C4" gate="G$1" pin="P$1"/>
<pinref part="GND8" gate="1" pin="0V"/>
</segment>
<segment>
<pinref part="U$1" gate="G$1" pin="GND"/>
<wire x1="-137.16" y1="236.22" x2="-139.7" y2="236.22" width="0.1524" layer="91"/>
<wire x1="-139.7" y1="236.22" x2="-139.7" y2="233.68" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="GND@1"/>
<wire x1="-139.7" y1="233.68" x2="-137.16" y2="233.68" width="0.1524" layer="91"/>
<pinref part="GND1" gate="1" pin="0V"/>
<wire x1="-139.7" y1="233.68" x2="-139.7" y2="232.41" width="0.1524" layer="91"/>
<junction x="-139.7" y="233.68"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="GND"/>
<wire x1="-137.16" y1="187.96" x2="-139.7" y2="187.96" width="0.1524" layer="91"/>
<wire x1="-139.7" y1="187.96" x2="-139.7" y2="185.42" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="GND@1"/>
<wire x1="-139.7" y1="185.42" x2="-137.16" y2="185.42" width="0.1524" layer="91"/>
<pinref part="GND2" gate="1" pin="0V"/>
<wire x1="-139.7" y1="185.42" x2="-139.7" y2="184.15" width="0.1524" layer="91"/>
<junction x="-139.7" y="185.42"/>
</segment>
<segment>
<pinref part="GND5" gate="1" pin="0V"/>
<pinref part="SW4" gate="G$1" pin="P$2"/>
<wire x1="-121.92" y1="133.35" x2="-121.92" y2="140.97" width="0.1524" layer="91"/>
</segment>
</net>
<net name="+5V" class="0">
<segment>
<pinref part="SK4" gate="G$1" pin="4"/>
<pinref part="P+3" gate="1" pin="+5V"/>
<wire x1="-231.14" y1="137.16" x2="-231.14" y2="147.32" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R16" gate="G$1" pin="2"/>
<pinref part="P+5" gate="1" pin="+5V"/>
<wire x1="-140.97" y1="139.7" x2="-137.16" y2="139.7" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="P$2"/>
<pinref part="P+6" gate="1" pin="+5V"/>
<wire x1="-304.8" y1="269.24" x2="-304.8" y2="266.7" width="0.1524" layer="91"/>
<pinref part="C3" gate="G$1" pin="P$2"/>
<wire x1="-304.8" y1="266.7" x2="-304.8" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-292.1" y1="264.16" x2="-292.1" y2="266.7" width="0.1524" layer="91"/>
<wire x1="-292.1" y1="266.7" x2="-304.8" y2="266.7" width="0.1524" layer="91"/>
<junction x="-304.8" y="266.7"/>
<pinref part="C8" gate="G$1" pin="P$2"/>
<wire x1="-276.86" y1="261.62" x2="-276.86" y2="266.7" width="0.1524" layer="91"/>
<wire x1="-276.86" y1="266.7" x2="-292.1" y2="266.7" width="0.1524" layer="91"/>
<junction x="-292.1" y="266.7"/>
<pinref part="C9" gate="G$1" pin="P$2"/>
<wire x1="-320.04" y1="261.62" x2="-320.04" y2="266.7" width="0.1524" layer="91"/>
<wire x1="-320.04" y1="266.7" x2="-304.8" y2="266.7" width="0.1524" layer="91"/>
<pinref part="C10" gate="G$1" pin="P$2"/>
<wire x1="-335.28" y1="261.62" x2="-335.28" y2="266.7" width="0.1524" layer="91"/>
<wire x1="-335.28" y1="266.7" x2="-320.04" y2="266.7" width="0.1524" layer="91"/>
<junction x="-320.04" y="266.7"/>
</segment>
<segment>
<pinref part="P+7" gate="1" pin="+5V"/>
<pinref part="C4" gate="G$1" pin="P$2"/>
<wire x1="-251.46" y1="144.78" x2="-251.46" y2="139.7" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$1" gate="G$1" pin="VCC"/>
<wire x1="-137.16" y1="241.3" x2="-139.7" y2="241.3" width="0.1524" layer="91"/>
<wire x1="-139.7" y1="241.3" x2="-139.7" y2="238.76" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="AVCC"/>
<wire x1="-139.7" y1="238.76" x2="-137.16" y2="238.76" width="0.1524" layer="91"/>
<pinref part="P+1" gate="1" pin="+5V"/>
<wire x1="-139.7" y1="241.3" x2="-139.7" y2="243.84" width="0.1524" layer="91"/>
<junction x="-139.7" y="241.3"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="VCC"/>
<wire x1="-137.16" y1="193.04" x2="-139.7" y2="193.04" width="0.1524" layer="91"/>
<wire x1="-139.7" y1="193.04" x2="-139.7" y2="190.5" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="AVCC"/>
<wire x1="-139.7" y1="190.5" x2="-137.16" y2="190.5" width="0.1524" layer="91"/>
<pinref part="P+2" gate="1" pin="+5V"/>
<wire x1="-139.7" y1="193.04" x2="-139.7" y2="194.31" width="0.1524" layer="91"/>
<junction x="-139.7" y="193.04"/>
</segment>
<segment>
<pinref part="R23" gate="G$1" pin="1"/>
<wire x1="-194.31" y1="156.21" x2="-198.12" y2="156.21" width="0.1524" layer="91"/>
<pinref part="P+4" gate="1" pin="+5V"/>
<wire x1="-198.12" y1="156.21" x2="-198.12" y2="157.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<wire x1="-167.64" y1="142.24" x2="-167.64" y2="144.78" width="0.1524" layer="91"/>
<wire x1="-167.64" y1="144.78" x2="-177.8" y2="144.78" width="0.1524" layer="91"/>
<pinref part="SK3" gate="G$1" pin="2"/>
<wire x1="-177.8" y1="144.78" x2="-177.8" y2="137.16" width="0.1524" layer="91"/>
<pinref part="R14" gate="G$1" pin="1"/>
<wire x1="-165.1" y1="142.24" x2="-167.64" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-177.8" y1="144.78" x2="-177.8" y2="154.94" width="0.1524" layer="91"/>
<junction x="-177.8" y="144.78"/>
<wire x1="-177.8" y1="154.94" x2="-151.13" y2="154.94" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="DI_PCINT0_PA0"/>
<wire x1="-137.16" y1="175.26" x2="-151.13" y2="175.26" width="0.1524" layer="91"/>
<wire x1="-151.13" y1="175.26" x2="-151.13" y2="223.52" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="DI_PCINT0_PA0"/>
<wire x1="-151.13" y1="223.52" x2="-137.16" y2="223.52" width="0.1524" layer="91"/>
<wire x1="-151.13" y1="154.94" x2="-151.13" y2="175.26" width="0.1524" layer="91"/>
<junction x="-151.13" y="175.26"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="SW1" gate="G$1" pin="P$1"/>
<wire x1="-226.06" y1="259.08" x2="-226.06" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R10" gate="G$1" pin="2"/>
<wire x1="-226.06" y1="264.16" x2="-227.33" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="SW2" gate="G$1" pin="P$1"/>
<wire x1="-203.2" y1="259.08" x2="-203.2" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R11" gate="G$1" pin="2"/>
<wire x1="-203.2" y1="264.16" x2="-207.01" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="SW3" gate="G$1" pin="P$1"/>
<wire x1="-177.8" y1="259.08" x2="-177.8" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R12" gate="G$1" pin="2"/>
<wire x1="-177.8" y1="264.16" x2="-181.61" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="C7" gate="G$1" pin="P$2"/>
<wire x1="-193.04" y1="261.62" x2="-193.04" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R12" gate="G$1" pin="1"/>
<wire x1="-193.04" y1="264.16" x2="-190.5" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-193.04" y1="264.16" x2="-193.04" y2="274.32" width="0.1524" layer="91"/>
<junction x="-193.04" y="264.16"/>
<wire x1="-193.04" y1="274.32" x2="-262.89" y2="274.32" width="0.1524" layer="91"/>
<wire x1="-262.89" y1="274.32" x2="-262.89" y2="207.01" width="0.1524" layer="91"/>
<wire x1="-262.89" y1="207.01" x2="-233.68" y2="207.01" width="0.1524" layer="91"/>
<pinref part="SK1" gate="G$1" pin="3"/>
<wire x1="-233.68" y1="207.01" x2="-233.68" y2="213.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="C5" gate="G$1" pin="P$2"/>
<wire x1="-238.76" y1="261.62" x2="-238.76" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R10" gate="G$1" pin="1"/>
<wire x1="-238.76" y1="264.16" x2="-236.22" y2="264.16" width="0.1524" layer="91"/>
<pinref part="SK2" gate="G$1" pin="3"/>
<wire x1="-304.8" y1="223.52" x2="-304.8" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-304.8" y1="208.28" x2="-265.43" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-265.43" y1="208.28" x2="-265.43" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-265.43" y1="264.16" x2="-238.76" y2="264.16" width="0.1524" layer="91"/>
<junction x="-238.76" y="264.16"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="SK1" gate="G$1" pin="8"/>
<wire x1="-246.38" y1="213.36" x2="-246.38" y2="209.55" width="0.1524" layer="91"/>
<wire x1="-246.38" y1="209.55" x2="-260.35" y2="209.55" width="0.1524" layer="91"/>
<wire x1="-260.35" y1="209.55" x2="-260.35" y2="271.78" width="0.1524" layer="91"/>
<wire x1="-260.35" y1="271.78" x2="-218.44" y2="271.78" width="0.1524" layer="91"/>
<pinref part="C6" gate="G$1" pin="P$2"/>
<wire x1="-218.44" y1="261.62" x2="-218.44" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R11" gate="G$1" pin="1"/>
<wire x1="-218.44" y1="264.16" x2="-215.9" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-218.44" y1="271.78" x2="-218.44" y2="264.16" width="0.1524" layer="91"/>
<junction x="-218.44" y="264.16"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="R15" gate="G$1" pin="2"/>
<wire x1="-156.21" y1="137.16" x2="-154.94" y2="137.16" width="0.1524" layer="91"/>
<wire x1="-154.94" y1="137.16" x2="-154.94" y2="139.7" width="0.1524" layer="91"/>
<pinref part="R14" gate="G$1" pin="2"/>
<wire x1="-154.94" y1="139.7" x2="-154.94" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-154.94" y1="142.24" x2="-156.21" y2="142.24" width="0.1524" layer="91"/>
<pinref part="R16" gate="G$1" pin="1"/>
<wire x1="-149.86" y1="139.7" x2="-152.4" y2="139.7" width="0.1524" layer="91"/>
<junction x="-154.94" y="139.7"/>
<pinref part="C1" gate="G$1" pin="P$2"/>
<wire x1="-152.4" y1="139.7" x2="-154.94" y2="139.7" width="0.1524" layer="91"/>
<wire x1="-152.4" y1="137.16" x2="-152.4" y2="139.7" width="0.1524" layer="91"/>
<junction x="-152.4" y="139.7"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="RESET_PB7"/>
<wire x1="-137.16" y1="220.98" x2="-148.59" y2="220.98" width="0.1524" layer="91"/>
<wire x1="-148.59" y1="220.98" x2="-148.59" y2="172.72" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="RESET_PB7"/>
<wire x1="-148.59" y1="172.72" x2="-137.16" y2="172.72" width="0.1524" layer="91"/>
<pinref part="SK3" gate="G$1" pin="3"/>
<wire x1="-180.34" y1="137.16" x2="-180.34" y2="156.21" width="0.1524" layer="91"/>
<wire x1="-180.34" y1="156.21" x2="-180.34" y2="157.48" width="0.1524" layer="91"/>
<wire x1="-180.34" y1="157.48" x2="-148.59" y2="157.48" width="0.1524" layer="91"/>
<wire x1="-148.59" y1="157.48" x2="-148.59" y2="172.72" width="0.1524" layer="91"/>
<junction x="-148.59" y="172.72"/>
<pinref part="R23" gate="G$1" pin="2"/>
<wire x1="-185.42" y1="156.21" x2="-180.34" y2="156.21" width="0.1524" layer="91"/>
<junction x="-180.34" y="156.21"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="SK3" gate="G$1" pin="1"/>
<wire x1="-175.26" y1="137.16" x2="-175.26" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-175.26" y1="142.24" x2="-170.18" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-170.18" y1="142.24" x2="-170.18" y2="137.16" width="0.1524" layer="91"/>
<pinref part="R15" gate="G$1" pin="1"/>
<wire x1="-165.1" y1="137.16" x2="-170.18" y2="137.16" width="0.1524" layer="91"/>
<wire x1="-175.26" y1="142.24" x2="-175.26" y2="152.4" width="0.1524" layer="91"/>
<junction x="-175.26" y="142.24"/>
<wire x1="-173.99" y1="152.4" x2="-172.72" y2="152.4" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="USCK_PA2"/>
<wire x1="-172.72" y1="152.4" x2="-146.05" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-137.16" y1="218.44" x2="-146.05" y2="218.44" width="0.1524" layer="91"/>
<wire x1="-146.05" y1="218.44" x2="-146.05" y2="170.18" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="USCK_PA2"/>
<wire x1="-146.05" y1="170.18" x2="-137.16" y2="170.18" width="0.1524" layer="91"/>
<wire x1="-146.05" y1="152.4" x2="-146.05" y2="170.18" width="0.1524" layer="91"/>
<junction x="-146.05" y="170.18"/>
<wire x1="-175.26" y1="152.4" x2="-173.99" y2="152.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="SW4" gate="G$1" pin="P$1"/>
<wire x1="-121.92" y1="146.05" x2="-121.92" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-121.92" y1="152.4" x2="-140.97" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-140.97" y1="152.4" x2="-140.97" y2="167.64" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="DO_PA1"/>
<wire x1="-140.97" y1="167.64" x2="-137.16" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<wire x1="-104.14" y1="241.3" x2="-104.14" y2="231.14" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="MOSI_PB0"/>
<wire x1="-109.22" y1="231.14" x2="-104.14" y2="231.14" width="0.1524" layer="91"/>
<wire x1="-104.14" y1="241.3" x2="-69.85" y2="241.3" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="DIG1"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PA6"/>
<wire x1="-109.22" y1="218.44" x2="-102.87" y2="218.44" width="0.1524" layer="91"/>
<wire x1="-102.87" y1="218.44" x2="-102.87" y2="231.14" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="DIG3"/>
<wire x1="-69.85" y1="231.14" x2="-102.87" y2="231.14" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PA5"/>
<wire x1="-109.22" y1="215.9" x2="-100.33" y2="215.9" width="0.1524" layer="91"/>
<wire x1="-100.33" y1="215.9" x2="-100.33" y2="233.68" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="DIG2"/>
<wire x1="-69.85" y1="233.68" x2="-100.33" y2="233.68" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PA3"/>
<wire x1="-109.22" y1="210.82" x2="-97.79" y2="210.82" width="0.1524" layer="91"/>
<wire x1="-97.79" y1="210.82" x2="-97.79" y2="238.76" width="0.1524" layer="91"/>
<pinref part="R2" gate="G$1" pin="1"/>
<wire x1="-97.79" y1="238.76" x2="-88.9" y2="238.76" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PB6"/>
<wire x1="-109.22" y1="241.3" x2="-107.95" y2="241.3" width="0.1524" layer="91"/>
<wire x1="-107.95" y1="241.3" x2="-107.95" y2="243.84" width="0.1524" layer="91"/>
<wire x1="-107.95" y1="243.84" x2="-93.98" y2="243.84" width="0.1524" layer="91"/>
<wire x1="-93.98" y1="243.84" x2="-93.98" y2="226.06" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="DIG4"/>
<wire x1="-93.98" y1="226.06" x2="-69.85" y2="226.06" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PB5"/>
<wire x1="-109.22" y1="238.76" x2="-105.41" y2="238.76" width="0.1524" layer="91"/>
<wire x1="-105.41" y1="238.76" x2="-105.41" y2="246.38" width="0.1524" layer="91"/>
<wire x1="-105.41" y1="246.38" x2="-91.44" y2="246.38" width="0.1524" layer="91"/>
<wire x1="-91.44" y1="223.52" x2="-91.44" y2="246.38" width="0.1524" layer="91"/>
<pinref part="R4" gate="G$1" pin="1"/>
<wire x1="-90.17" y1="223.52" x2="-91.44" y2="223.52" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PB4"/>
<wire x1="-109.22" y1="236.22" x2="-106.68" y2="236.22" width="0.1524" layer="91"/>
<wire x1="-106.68" y1="236.22" x2="-106.68" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-106.68" y1="251.46" x2="-54.61" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-54.61" y1="251.46" x2="-54.61" y2="203.2" width="0.1524" layer="91"/>
<wire x1="-54.61" y1="203.2" x2="-90.17" y2="203.2" width="0.1524" layer="91"/>
<pinref part="R7" gate="G$1" pin="1"/>
<wire x1="-80.01" y1="220.98" x2="-90.17" y2="220.98" width="0.1524" layer="91"/>
<wire x1="-90.17" y1="203.2" x2="-90.17" y2="220.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PB3"/>
<wire x1="-109.22" y1="233.68" x2="-101.6" y2="233.68" width="0.1524" layer="91"/>
<wire x1="-101.6" y1="233.68" x2="-101.6" y2="254" width="0.1524" layer="91"/>
<wire x1="-101.6" y1="254" x2="-52.07" y2="254" width="0.1524" layer="91"/>
<wire x1="-52.07" y1="254" x2="-52.07" y2="200.66" width="0.1524" layer="91"/>
<wire x1="-52.07" y1="200.66" x2="-92.71" y2="200.66" width="0.1524" layer="91"/>
<wire x1="-92.71" y1="200.66" x2="-92.71" y2="218.44" width="0.1524" layer="91"/>
<pinref part="R8" gate="G$1" pin="1"/>
<wire x1="-88.9" y1="218.44" x2="-92.71" y2="218.44" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="SCK_PB2"/>
<wire x1="-109.22" y1="226.06" x2="-96.52" y2="226.06" width="0.1524" layer="91"/>
<wire x1="-96.52" y1="226.06" x2="-96.52" y2="256.54" width="0.1524" layer="91"/>
<wire x1="-96.52" y1="256.54" x2="-49.53" y2="256.54" width="0.1524" layer="91"/>
<wire x1="-49.53" y1="256.54" x2="-49.53" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-49.53" y1="208.28" x2="-93.98" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-93.98" y1="208.28" x2="-93.98" y2="215.9" width="0.1524" layer="91"/>
<pinref part="R5" gate="G$1" pin="1"/>
<wire x1="-80.01" y1="215.9" x2="-93.98" y2="215.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PA4"/>
<wire x1="-109.22" y1="213.36" x2="-99.06" y2="213.36" width="0.1524" layer="91"/>
<wire x1="-99.06" y1="213.36" x2="-99.06" y2="236.22" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="1"/>
<wire x1="-81.28" y1="236.22" x2="-99.06" y2="236.22" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="MISO_PB1"/>
<wire x1="-109.22" y1="228.6" x2="-106.68" y2="228.6" width="0.1524" layer="91"/>
<wire x1="-106.68" y1="228.6" x2="-106.68" y2="207.01" width="0.1524" layer="91"/>
<wire x1="-106.68" y1="207.01" x2="-96.52" y2="207.01" width="0.1524" layer="91"/>
<wire x1="-96.52" y1="213.36" x2="-96.52" y2="207.01" width="0.1524" layer="91"/>
<pinref part="R6" gate="G$1" pin="1"/>
<wire x1="-88.9" y1="213.36" x2="-96.52" y2="213.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="PA7"/>
<wire x1="-109.22" y1="220.98" x2="-101.6" y2="220.98" width="0.1524" layer="91"/>
<wire x1="-101.6" y1="228.6" x2="-101.6" y2="220.98" width="0.1524" layer="91"/>
<pinref part="R3" gate="G$1" pin="1"/>
<wire x1="-80.01" y1="228.6" x2="-101.6" y2="228.6" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="PA4"/>
<wire x1="-109.22" y1="165.1" x2="-102.87" y2="165.1" width="0.1524" layer="91"/>
<wire x1="-102.87" y1="165.1" x2="-102.87" y2="167.64" width="0.1524" layer="91"/>
<pinref part="R20" gate="G$1" pin="1"/>
<wire x1="-78.74" y1="167.64" x2="-102.87" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG4"/>
<wire x1="-105.41" y1="177.8" x2="-67.31" y2="177.8" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="PB6"/>
<wire x1="-109.22" y1="193.04" x2="-105.41" y2="193.04" width="0.1524" layer="91"/>
<wire x1="-105.41" y1="193.04" x2="-105.41" y2="177.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<wire x1="-106.68" y1="175.26" x2="-106.68" y2="172.72" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="PA7"/>
<wire x1="-106.68" y1="172.72" x2="-109.22" y2="172.72" width="0.1524" layer="91"/>
<pinref part="R18" gate="G$1" pin="1"/>
<wire x1="-82.55" y1="175.26" x2="-106.68" y2="175.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$28" class="0">
<segment>
<wire x1="-105.41" y1="172.72" x2="-105.41" y2="170.18" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="PA6"/>
<wire x1="-105.41" y1="170.18" x2="-109.22" y2="170.18" width="0.1524" layer="91"/>
<pinref part="R19" gate="G$1" pin="1"/>
<wire x1="-93.98" y1="172.72" x2="-105.41" y2="172.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<wire x1="-104.14" y1="170.18" x2="-104.14" y2="167.64" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="PA5"/>
<wire x1="-104.14" y1="167.64" x2="-109.22" y2="167.64" width="0.1524" layer="91"/>
<pinref part="R22" gate="G$1" pin="1"/>
<wire x1="-104.14" y1="170.18" x2="-101.6" y2="170.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$31" class="0">
<segment>
<wire x1="-100.33" y1="165.1" x2="-100.33" y2="162.56" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="PA3"/>
<wire x1="-100.33" y1="162.56" x2="-109.22" y2="162.56" width="0.1524" layer="91"/>
<wire x1="-100.33" y1="165.1" x2="-86.36" y2="165.1" width="0.1524" layer="91"/>
<pinref part="R21" gate="G$1" pin="1"/>
</segment>
</net>
<net name="N$32" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="PB5"/>
<wire x1="-109.22" y1="190.5" x2="-104.14" y2="190.5" width="0.1524" layer="91"/>
<wire x1="-104.14" y1="190.5" x2="-104.14" y2="193.04" width="0.1524" layer="91"/>
<pinref part="U$4" gate="G$1" pin="DIG1"/>
<wire x1="-104.14" y1="193.04" x2="-67.31" y2="193.04" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$33" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="PB4"/>
<wire x1="-109.22" y1="187.96" x2="-102.87" y2="187.96" width="0.1524" layer="91"/>
<wire x1="-102.87" y1="187.96" x2="-102.87" y2="190.5" width="0.1524" layer="91"/>
<pinref part="R9" gate="G$1" pin="1"/>
<wire x1="-80.01" y1="190.5" x2="-102.87" y2="190.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$34" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="PB3"/>
<wire x1="-109.22" y1="185.42" x2="-101.6" y2="185.42" width="0.1524" layer="91"/>
<wire x1="-101.6" y1="185.42" x2="-101.6" y2="187.96" width="0.1524" layer="91"/>
<pinref part="R13" gate="G$1" pin="1"/>
<wire x1="-101.6" y1="187.96" x2="-92.71" y2="187.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$35" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG2"/>
<wire x1="-67.31" y1="185.42" x2="-100.33" y2="185.42" width="0.1524" layer="91"/>
<wire x1="-100.33" y1="185.42" x2="-100.33" y2="176.53" width="0.1524" layer="91"/>
<wire x1="-100.33" y1="176.53" x2="-107.95" y2="176.53" width="0.1524" layer="91"/>
<wire x1="-107.95" y1="176.53" x2="-107.95" y2="177.8" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="SCK_PB2"/>
<wire x1="-107.95" y1="177.8" x2="-109.22" y2="177.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$36" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="MISO_PB1"/>
<wire x1="-109.22" y1="180.34" x2="-76.2" y2="180.34" width="0.1524" layer="91"/>
<wire x1="-76.2" y1="180.34" x2="-76.2" y2="182.88" width="0.1524" layer="91"/>
<pinref part="U$4" gate="G$1" pin="DIG3"/>
<wire x1="-76.2" y1="182.88" x2="-67.31" y2="182.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="2"/>
<pinref part="U$3" gate="G$1" pin="A"/>
<wire x1="-80.01" y1="238.76" x2="-69.85" y2="238.76" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="G"/>
<pinref part="R4" gate="G$1" pin="2"/>
<wire x1="-69.85" y1="223.52" x2="-81.28" y2="223.52" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$38" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="B"/>
<pinref part="R3" gate="G$1" pin="2"/>
<wire x1="-69.85" y1="228.6" x2="-71.12" y2="228.6" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$39" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="F"/>
<pinref part="R1" gate="G$1" pin="2"/>
<wire x1="-69.85" y1="236.22" x2="-72.39" y2="236.22" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$40" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="C"/>
<pinref part="R7" gate="G$1" pin="2"/>
<wire x1="-71.12" y1="220.98" x2="-69.85" y2="220.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$42" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="E"/>
<pinref part="R6" gate="G$1" pin="2"/>
<wire x1="-69.85" y1="213.36" x2="-80.01" y2="213.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$43" class="0">
<segment>
<pinref part="R8" gate="G$1" pin="2"/>
<pinref part="U$3" gate="G$1" pin="CLON"/>
<wire x1="-80.01" y1="218.44" x2="-69.85" y2="218.44" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$44" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="MOSI_PB0"/>
<pinref part="R17" gate="G$1" pin="1"/>
<wire x1="-109.22" y1="182.88" x2="-96.52" y2="182.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$45" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="A"/>
<pinref part="R9" gate="G$1" pin="2"/>
<wire x1="-67.31" y1="190.5" x2="-71.12" y2="190.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$46" class="0">
<segment>
<pinref part="R13" gate="G$1" pin="2"/>
<pinref part="U$4" gate="G$1" pin="F"/>
<wire x1="-83.82" y1="187.96" x2="-67.31" y2="187.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$47" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="G"/>
<pinref part="R18" gate="G$1" pin="2"/>
<wire x1="-67.31" y1="175.26" x2="-73.66" y2="175.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$48" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="C"/>
<pinref part="R19" gate="G$1" pin="2"/>
<wire x1="-67.31" y1="172.72" x2="-85.09" y2="172.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$49" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="D"/>
<pinref part="R20" gate="G$1" pin="2"/>
<wire x1="-67.31" y1="167.64" x2="-69.85" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$37" class="0">
<segment>
<wire x1="-72.39" y1="184.15" x2="-72.39" y2="180.34" width="0.1524" layer="91"/>
<pinref part="U$4" gate="G$1" pin="B"/>
<wire x1="-72.39" y1="180.34" x2="-67.31" y2="180.34" width="0.1524" layer="91"/>
<wire x1="-72.39" y1="184.15" x2="-85.09" y2="184.15" width="0.1524" layer="91"/>
<wire x1="-85.09" y1="184.15" x2="-85.09" y2="182.88" width="0.1524" layer="91"/>
<pinref part="R17" gate="G$1" pin="2"/>
<wire x1="-85.09" y1="182.88" x2="-87.63" y2="182.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$50" class="0">
<segment>
<pinref part="R21" gate="G$1" pin="2"/>
<pinref part="U$4" gate="G$1" pin="E"/>
<wire x1="-77.47" y1="165.1" x2="-67.31" y2="165.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$51" class="0">
<segment>
<pinref part="R22" gate="G$1" pin="2"/>
<pinref part="U$4" gate="G$1" pin="CLON"/>
<wire x1="-92.71" y1="170.18" x2="-67.31" y2="170.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$41" class="0">
<segment>
<pinref part="R5" gate="G$1" pin="2"/>
<pinref part="U$3" gate="G$1" pin="D"/>
<wire x1="-71.12" y1="215.9" x2="-69.85" y2="215.9" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
