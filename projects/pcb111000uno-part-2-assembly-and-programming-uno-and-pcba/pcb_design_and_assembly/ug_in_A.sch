<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.0.1">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
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
<package name="DIP28">
<pad name="P$1" x="0" y="0" drill="0.6" shape="square"/>
<pad name="P$2" x="0" y="-2.54" drill="0.6" shape="square"/>
<pad name="P$3" x="0" y="-5.08" drill="0.6" shape="square"/>
<pad name="P$4" x="0" y="-7.62" drill="0.6" shape="square"/>
<pad name="P$5" x="0" y="-10.16" drill="0.6" shape="square"/>
<pad name="P$6" x="0" y="-12.7" drill="0.6" shape="square"/>
<pad name="P$7" x="0" y="-15.24" drill="0.6" shape="square"/>
<pad name="P$8" x="0" y="-17.78" drill="0.6" shape="square"/>
<pad name="P$9" x="0" y="-20.32" drill="0.6" shape="square"/>
<pad name="P$10" x="0" y="-22.86" drill="0.6" shape="square"/>
<pad name="P$11" x="0" y="-25.4" drill="0.6" shape="square"/>
<pad name="P$12" x="0" y="-27.94" drill="0.6" shape="square"/>
<pad name="P$13" x="0" y="-30.48" drill="0.6" shape="square"/>
<pad name="P$14" x="0" y="-33.02" drill="0.6" shape="square"/>
<pad name="P$15" x="7.62" y="-33.02" drill="0.6" shape="square"/>
<pad name="P$16" x="7.62" y="-30.48" drill="0.6" shape="square"/>
<pad name="P$17" x="7.62" y="-27.94" drill="0.6" shape="square"/>
<pad name="P$18" x="7.62" y="-25.4" drill="0.6" shape="square"/>
<pad name="P$19" x="7.62" y="-22.86" drill="0.6" shape="square"/>
<pad name="P$20" x="7.62" y="-20.32" drill="0.6" shape="square"/>
<pad name="P$21" x="7.62" y="-17.78" drill="0.6" shape="square"/>
<pad name="P$22" x="7.62" y="-15.24" drill="0.6" shape="square"/>
<pad name="P$23" x="7.62" y="-12.7" drill="0.6" shape="square"/>
<pad name="P$24" x="7.62" y="-10.16" drill="0.6" shape="square"/>
<pad name="P$25" x="7.62" y="-7.62" drill="0.6" shape="square"/>
<pad name="P$26" x="7.62" y="-5.08" drill="0.6" shape="square"/>
<pad name="P$27" x="7.62" y="-2.54" drill="0.6" shape="square"/>
<pad name="P$28" x="7.62" y="0" drill="0.6" shape="square"/>
<wire x1="-1.016" y1="1.016" x2="-1.016" y2="-34.036" width="0.127" layer="21"/>
<wire x1="-1.016" y1="-34.036" x2="8.636" y2="-34.036" width="0.127" layer="21"/>
<wire x1="8.636" y1="-34.036" x2="8.636" y2="1.016" width="0.127" layer="21"/>
<wire x1="-1.016" y1="1.016" x2="3.048" y2="1.016" width="0.127" layer="21"/>
<wire x1="3.048" y1="1.016" x2="3.048" y2="0" width="0.127" layer="21"/>
<wire x1="3.048" y1="0" x2="4.572" y2="0" width="0.127" layer="21"/>
<wire x1="4.572" y1="0" x2="4.572" y2="1.016" width="0.127" layer="21"/>
<wire x1="4.572" y1="1.016" x2="8.636" y2="1.016" width="0.127" layer="21"/>
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
<package name="DUAL_LED">
<pad name="P$1" x="0" y="2.54" drill="1" shape="square"/>
<pad name="P$2" x="0" y="0" drill="1" shape="square"/>
<pad name="P$3" x="0" y="-2.54" drill="1" shape="square"/>
<circle x="0" y="0" radius="3.304440625" width="0.127" layer="21"/>
</package>
<package name="PUSH_BUTTON_NO_SM">
<smd name="P$1" x="-0.03" y="-0.78" dx="3" dy="4.5" layer="1" rot="R180"/>
<smd name="P$2" x="-0.03" y="8.42" dx="3" dy="4.5" layer="1" rot="R180"/>
</package>
<package name="PB_NO_TH">
<smd name="P$1" x="0" y="0" dx="2" dy="4" layer="1"/>
<smd name="P$2" x="6.75" y="0" dx="2" dy="4" layer="1"/>
</package>
<package name="CRYSTAL">
<pad name="P$1" x="0" y="0" drill="0.6" shape="square"/>
<pad name="P$2" x="0" y="-2.54" drill="0.6" shape="square"/>
</package>
<package name="DIL28-3" urn="urn:adsk.eagle:footprint:4162/1">
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
<package name="CA_LINK">
<pad name="P$1" x="0" y="2.54" drill="0.6" shape="square"/>
<pad name="P$2" x="0" y="0" drill="0.6" shape="square"/>
<text x="-1.27" y="-1.905" size="1.27" layer="21" rot="R90">&gt;NAME</text>
</package>
</packages>
<packages3d>
<package3d name="DIL28-3" urn="urn:adsk.eagle:package:4320/1" type="box">
<description>Dual In Line 0.3 inch</description>
<packageinstances>
<packageinstance name="DIL28-3"/>
</packageinstances>
</package3d>
</packages3d>
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
<symbol name="ATMEGA328">
<pin name="C6" x="-2.54" y="0" visible="pin" length="short"/>
<pin name="D0" x="-2.54" y="-2.54" visible="pin" length="short"/>
<pin name="D1" x="-2.54" y="-5.08" visible="pin" length="short"/>
<pin name="D2" x="-2.54" y="-7.62" visible="pin" length="short"/>
<pin name="D3" x="-2.54" y="-10.16" visible="pin" length="short"/>
<pin name="D4" x="-2.54" y="-12.7" visible="pin" length="short"/>
<pin name="5V" x="-2.54" y="-15.24" visible="pin" length="short"/>
<pin name="0V" x="-2.54" y="-17.78" visible="pin" length="short"/>
<pin name="B6" x="-2.54" y="-20.32" visible="pin" length="short"/>
<pin name="B7" x="-2.54" y="-22.86" visible="pin" length="short"/>
<pin name="D5" x="-2.54" y="-25.4" visible="pin" length="short"/>
<pin name="D6" x="-2.54" y="-27.94" visible="pin" length="short"/>
<pin name="D7" x="-2.54" y="-30.48" visible="pin" length="short"/>
<pin name="B0" x="-2.54" y="-33.02" visible="pin" length="short"/>
<pin name="B1" x="17.78" y="-33.02" visible="pin" length="short" rot="R180"/>
<pin name="B2" x="17.78" y="-30.48" visible="pin" length="short" rot="R180"/>
<pin name="B3" x="17.78" y="-27.94" visible="pin" length="short" rot="R180"/>
<pin name="B4" x="17.78" y="-25.4" visible="pin" length="short" rot="R180"/>
<pin name="B5" x="17.78" y="-22.86" visible="pin" length="short" rot="R180"/>
<pin name="_5V" x="17.78" y="-20.32" visible="pin" length="short" rot="R180"/>
<pin name="REF" x="17.78" y="-17.78" visible="pin" length="short" rot="R180"/>
<pin name="_0V" x="17.78" y="-15.24" visible="pin" length="short" rot="R180"/>
<pin name="C0" x="17.78" y="-12.7" visible="pin" length="short" rot="R180"/>
<pin name="C1" x="17.78" y="-10.16" visible="pin" length="short" rot="R180"/>
<pin name="C2" x="17.78" y="-7.62" visible="pin" length="short" rot="R180"/>
<pin name="C3" x="17.78" y="-5.08" visible="pin" length="short" rot="R180"/>
<pin name="C4" x="17.78" y="-2.54" visible="pin" length="short" rot="R180"/>
<pin name="C5" x="17.78" y="0" visible="pin" length="short" rot="R180"/>
<wire x1="0" y1="2.54" x2="0" y2="-35.56" width="0.254" layer="94"/>
<wire x1="0" y1="-35.56" x2="15.24" y2="-35.56" width="0.254" layer="94"/>
<wire x1="15.24" y1="-35.56" x2="15.24" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="6.604" y2="2.54" width="0.254" layer="94"/>
<wire x1="6.604" y1="2.54" x2="6.604" y2="1.778" width="0.254" layer="94"/>
<wire x1="6.604" y1="1.778" x2="7.874" y2="1.778" width="0.254" layer="94"/>
<wire x1="7.874" y1="1.778" x2="7.874" y2="2.54" width="0.254" layer="94"/>
<wire x1="7.874" y1="2.54" x2="15.24" y2="2.54" width="0.254" layer="94"/>
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
<symbol name="CC_LED">
<pin name="P$1" x="0" y="0" visible="off" length="short"/>
<wire x1="2.54" y1="2.54" x2="2.54" y2="-2.54" width="0.254" layer="94"/>
<pin name="P$2" x="7.62" y="2.54" visible="off" length="short" rot="R180"/>
<pin name="P$3" x="7.62" y="-2.54" visible="off" length="short" rot="R180"/>
<wire x1="3.81" y1="2.54" x2="5.334" y2="3.81" width="0.254" layer="94"/>
<wire x1="5.334" y1="3.81" x2="5.334" y2="1.27" width="0.254" layer="94"/>
<wire x1="5.334" y1="1.27" x2="3.81" y2="2.54" width="0.254" layer="94"/>
<wire x1="3.81" y1="1.27" x2="3.81" y2="2.54" width="0.254" layer="94"/>
<wire x1="3.81" y1="2.54" x2="3.81" y2="3.81" width="0.254" layer="94"/>
<wire x1="3.81" y1="2.54" x2="2.54" y2="2.54" width="0.254" layer="94"/>
<wire x1="5.334" y1="-4.064" x2="5.334" y2="-1.016" width="0.254" layer="94"/>
<wire x1="5.334" y1="-1.016" x2="3.556" y2="-2.54" width="0.254" layer="94"/>
<wire x1="3.556" y1="-2.54" x2="5.334" y2="-4.064" width="0.254" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="3.556" y2="-2.54" width="0.254" layer="94"/>
<wire x1="3.556" y1="-1.016" x2="3.556" y2="-4.064" width="0.254" layer="94"/>
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
<symbol name="CRYSTAL">
<pin name="P$1" x="0" y="1.27" visible="off" length="short"/>
<pin name="P$2" x="0" y="0" visible="off" length="short"/>
<wire x1="2.54" y1="1.905" x2="7.62" y2="1.905" width="0.254" layer="94"/>
<wire x1="7.62" y1="1.905" x2="7.62" y2="-0.635" width="0.254" layer="94"/>
<wire x1="7.62" y1="-0.635" x2="2.54" y2="-0.635" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.635" x2="2.54" y2="1.905" width="0.254" layer="94"/>
</symbol>
<symbol name="CA_LINK">
<pin name="P$1" x="0" y="0" visible="off" length="short"/>
<pin name="P$2" x="0" y="5.08" visible="off" length="short"/>
<text x="-2.54" y="0" size="1.778" layer="94" rot="R90">&gt;NAME</text>
<circle x="2.54" y="5.08" radius="0.915809375" width="0.254" layer="94"/>
<circle x="2.54" y="0" radius="0.915809375" width="0.254" layer="94"/>
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
<deviceset name="ATMEGA328">
<gates>
<gate name="G$1" symbol="ATMEGA328" x="-5.08" y="111.76"/>
</gates>
<devices>
<device name="" package="DIP28">
<connects>
<connect gate="G$1" pin="0V" pad="P$8"/>
<connect gate="G$1" pin="5V" pad="P$7"/>
<connect gate="G$1" pin="B0" pad="P$14"/>
<connect gate="G$1" pin="B1" pad="P$15"/>
<connect gate="G$1" pin="B2" pad="P$16"/>
<connect gate="G$1" pin="B3" pad="P$17"/>
<connect gate="G$1" pin="B4" pad="P$18"/>
<connect gate="G$1" pin="B5" pad="P$19"/>
<connect gate="G$1" pin="B6" pad="P$9"/>
<connect gate="G$1" pin="B7" pad="P$10"/>
<connect gate="G$1" pin="C0" pad="P$23"/>
<connect gate="G$1" pin="C1" pad="P$24"/>
<connect gate="G$1" pin="C2" pad="P$25"/>
<connect gate="G$1" pin="C3" pad="P$26"/>
<connect gate="G$1" pin="C4" pad="P$27"/>
<connect gate="G$1" pin="C5" pad="P$28"/>
<connect gate="G$1" pin="C6" pad="P$1"/>
<connect gate="G$1" pin="D0" pad="P$2"/>
<connect gate="G$1" pin="D1" pad="P$3"/>
<connect gate="G$1" pin="D2" pad="P$4"/>
<connect gate="G$1" pin="D3" pad="P$5"/>
<connect gate="G$1" pin="D4" pad="P$6"/>
<connect gate="G$1" pin="D5" pad="P$11"/>
<connect gate="G$1" pin="D6" pad="P$12"/>
<connect gate="G$1" pin="D7" pad="P$13"/>
<connect gate="G$1" pin="REF" pad="P$21"/>
<connect gate="G$1" pin="_0V" pad="P$22"/>
<connect gate="G$1" pin="_5V" pad="P$20"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="DIP28_3" package="DIL28-3">
<connects>
<connect gate="G$1" pin="0V" pad="8"/>
<connect gate="G$1" pin="5V" pad="7"/>
<connect gate="G$1" pin="B0" pad="14"/>
<connect gate="G$1" pin="B1" pad="15"/>
<connect gate="G$1" pin="B2" pad="16"/>
<connect gate="G$1" pin="B3" pad="17"/>
<connect gate="G$1" pin="B4" pad="18"/>
<connect gate="G$1" pin="B5" pad="19"/>
<connect gate="G$1" pin="B6" pad="9"/>
<connect gate="G$1" pin="B7" pad="10"/>
<connect gate="G$1" pin="C0" pad="23"/>
<connect gate="G$1" pin="C1" pad="24"/>
<connect gate="G$1" pin="C2" pad="25"/>
<connect gate="G$1" pin="C3" pad="26"/>
<connect gate="G$1" pin="C4" pad="27"/>
<connect gate="G$1" pin="C5" pad="28"/>
<connect gate="G$1" pin="C6" pad="1"/>
<connect gate="G$1" pin="D0" pad="2"/>
<connect gate="G$1" pin="D1" pad="3"/>
<connect gate="G$1" pin="D2" pad="4"/>
<connect gate="G$1" pin="D3" pad="5"/>
<connect gate="G$1" pin="D4" pad="6"/>
<connect gate="G$1" pin="D5" pad="11"/>
<connect gate="G$1" pin="D6" pad="12"/>
<connect gate="G$1" pin="D7" pad="13"/>
<connect gate="G$1" pin="REF" pad="21"/>
<connect gate="G$1" pin="_0V" pad="22"/>
<connect gate="G$1" pin="_5V" pad="20"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:4320/1"/>
</package3dinstances>
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
<deviceset name="CC_LED">
<gates>
<gate name="G$1" symbol="CC_LED" x="-5.08" y="58.42"/>
</gates>
<devices>
<device name="" package="DUAL_LED">
<connects>
<connect gate="G$1" pin="P$1" pad="P$2"/>
<connect gate="G$1" pin="P$2" pad="P$1"/>
<connect gate="G$1" pin="P$3" pad="P$3"/>
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
<deviceset name="CRYSTAL">
<gates>
<gate name="G$1" symbol="CRYSTAL" x="-5.08" y="10.16"/>
</gates>
<devices>
<device name="" package="CRYSTAL">
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
<deviceset name="CA_LINK">
<gates>
<gate name="G$1" symbol="CA_LINK" x="-17.78" y="50.8"/>
</gates>
<devices>
<device name="" package="CA_LINK">
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
<part name="U$2" library="1_home_library" deviceset="ATMEGA328" device="DIP28_3" package3d_urn="urn:adsk.eagle:package:4320/1" value="ATMEGA328DIP28_3"/>
<part name="R1" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R2" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R3" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R4" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R5" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R6" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R7" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="U$3" library="1_home_library" deviceset="DISPLAY" device=""/>
<part name="U$4" library="1_home_library" deviceset="DISPLAY" device=""/>
<part name="U$5" library="1_home_library" deviceset="CC_LED" device=""/>
<part name="R8" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="R9" library="1_home_library" deviceset="RESISTOR" device="" value="560R"/>
<part name="P+1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="GND1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="GND2" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
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
<part name="R13" library="1_home_library" deviceset="RESISTOR" device="" value="10K"/>
<part name="P+4" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="+5V" device=""/>
<part name="GND5" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="0V" device=""/>
<part name="U$16" library="1_home_library" deviceset="CRYSTAL" device=""/>
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
<part name="R17" library="1_home_library" deviceset="RESISTOR" device="" value="1K"/>
<part name="CC" library="1_home_library" deviceset="CA_LINK" device=""/>
<part name="CA" library="1_home_library" deviceset="CA_LINK" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="-162.56" y="193.04" size="1.778" layer="91">560R ea.</text>
</plain>
<instances>
<instance part="SK3" gate="G$1" x="-175.26" y="137.16" smashed="yes" rot="R270">
<attribute name="NAME" x="-187.96" y="139.7" size="1.778" layer="94"/>
</instance>
<instance part="SK1" gate="G$1" x="-157.48" y="213.36" smashed="yes" rot="MR90">
<attribute name="NAME" x="-157.48" y="220.98" size="1.778" layer="94" rot="MR0"/>
</instance>
<instance part="SK2" gate="G$1" x="-228.6" y="223.52" smashed="yes" rot="MR90">
<attribute name="NAME" x="-228.6" y="231.14" size="1.778" layer="94" rot="MR0"/>
</instance>
<instance part="SK4" gate="G$1" x="-223.52" y="137.16" smashed="yes" rot="R270">
<attribute name="NAME" x="-228.6" y="127" size="1.778" layer="94"/>
</instance>
<instance part="U$2" gate="G$1" x="-256.54" y="167.64" rot="R90"/>
<instance part="R1" gate="G$1" x="-160.02" y="190.5" smashed="yes">
<attribute name="NAME" x="-161.29" y="191.008" size="1.778" layer="94"/>
</instance>
<instance part="R2" gate="G$1" x="-160.02" y="187.96" smashed="yes">
<attribute name="NAME" x="-161.29" y="188.214" size="1.778" layer="94"/>
</instance>
<instance part="R3" gate="G$1" x="-160.02" y="185.42" smashed="yes">
<attribute name="NAME" x="-161.544" y="185.674" size="1.778" layer="94"/>
</instance>
<instance part="R4" gate="G$1" x="-160.02" y="182.88" smashed="yes">
<attribute name="NAME" x="-162.052" y="183.134" size="1.778" layer="94"/>
</instance>
<instance part="R5" gate="G$1" x="-160.02" y="180.34" smashed="yes">
<attribute name="NAME" x="-161.544" y="180.594" size="1.778" layer="94"/>
</instance>
<instance part="R6" gate="G$1" x="-160.02" y="177.8" smashed="yes">
<attribute name="NAME" x="-161.544" y="178.054" size="1.778" layer="94"/>
</instance>
<instance part="R7" gate="G$1" x="-160.02" y="175.26" smashed="yes">
<attribute name="NAME" x="-161.29" y="175.768" size="1.778" layer="94"/>
</instance>
<instance part="U$3" gate="G$1" x="-119.38" y="213.36"/>
<instance part="U$4" gate="G$1" x="-119.38" y="167.64"/>
<instance part="U$5" gate="G$1" x="-203.2" y="218.44" rot="R180"/>
<instance part="R8" gate="G$1" x="-215.9" y="215.9" smashed="yes" rot="R180">
<attribute name="NAME" x="-219.202" y="214.376" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-212.09" y="214.63" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="R9" gate="G$1" x="-215.9" y="220.98" smashed="yes" rot="R180">
<attribute name="NAME" x="-223.012" y="224.536" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-215.9" y="224.79" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="P+1" gate="1" x="-264.16" y="210.82"/>
<instance part="GND1" gate="1" x="-198.12" y="213.36"/>
<instance part="GND2" gate="1" x="-276.86" y="149.86"/>
<instance part="GND3" gate="1" x="-210.82" y="139.7"/>
<instance part="P+3" gate="1" x="-231.14" y="149.86"/>
<instance part="SW4" gate="G$1" x="-187.96" y="175.26" smashed="yes" rot="R270">
<attribute name="NAME" x="-185.42" y="172.72" size="1.778" layer="94"/>
</instance>
<instance part="R10" gate="G$1" x="-165.1" y="264.16" smashed="yes">
<attribute name="NAME" x="-161.798" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-162.56" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="R11" gate="G$1" x="-144.78" y="264.16" smashed="yes">
<attribute name="NAME" x="-141.478" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-142.24" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="R12" gate="G$1" x="-119.38" y="264.16" smashed="yes">
<attribute name="NAME" x="-116.078" y="265.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-116.84" y="261.62" size="1.778" layer="94"/>
</instance>
<instance part="C5" gate="G$1" x="-167.64" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-170.18" y="259.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-170.18" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C6" gate="G$1" x="-147.32" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-148.59" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-142.24" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C7" gate="G$1" x="-121.92" y="254" smashed="yes" rot="R90">
<attribute name="NAME" x="-123.19" y="256.54" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-116.84" y="256.54" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="SW3" gate="G$1" x="-106.68" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-114.3" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="SW2" gate="G$1" x="-132.08" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-139.7" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="SW1" gate="G$1" x="-154.94" y="259.08" smashed="yes" rot="R270">
<attribute name="NAME" x="-162.56" y="256.54" size="1.778" layer="94"/>
</instance>
<instance part="GND4" gate="1" x="-106.68" y="243.84"/>
<instance part="R13" gate="G$1" x="-200.66" y="170.18" smashed="yes" rot="R90">
<attribute name="NAME" x="-202.692" y="173.736" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-203.2" y="171.45" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="P+4" gate="1" x="-200.66" y="187.96"/>
<instance part="GND5" gate="1" x="-187.96" y="182.88" rot="R180"/>
<instance part="U$16" gate="G$1" x="-231.14" y="157.48"/>
<instance part="R14" gate="G$1" x="-165.1" y="142.24" smashed="yes">
<attribute name="NAME" x="-161.798" y="143.764" size="1.778" layer="94"/>
<attribute name="VALUE" x="-156.21" y="143.51" size="1.778" layer="94"/>
</instance>
<instance part="R15" gate="G$1" x="-165.1" y="137.16" smashed="yes">
<attribute name="NAME" x="-161.798" y="138.684" size="1.778" layer="94"/>
<attribute name="VALUE" x="-162.56" y="134.62" size="1.778" layer="94"/>
</instance>
<instance part="R16" gate="G$1" x="-149.86" y="139.7"/>
<instance part="C1" gate="G$1" x="-152.4" y="129.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-153.67" y="132.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-146.05" y="132.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="GND6" gate="1" x="-152.4" y="124.46"/>
<instance part="P+5" gate="1" x="-134.62" y="139.7" rot="R270"/>
<instance part="C2" gate="G$1" x="-233.68" y="256.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-238.76" y="259.08" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-233.68" y="259.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C3" gate="G$1" x="-220.98" y="256.54" smashed="yes" rot="R90">
<attribute name="NAME" x="-222.25" y="262.89" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-219.71" y="259.08" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="C4" gate="G$1" x="-251.46" y="132.08" smashed="yes" rot="R90">
<attribute name="NAME" x="-255.27" y="137.16" size="1.778" layer="94" rot="R180"/>
<attribute name="VALUE" x="-255.27" y="134.62" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="P+6" gate="1" x="-233.68" y="271.78"/>
<instance part="GND7" gate="1" x="-233.68" y="243.84"/>
<instance part="GND8" gate="1" x="-251.46" y="129.54"/>
<instance part="P+7" gate="1" x="-251.46" y="147.32"/>
<instance part="R17" gate="G$1" x="-187.96" y="157.48" rot="R90"/>
<instance part="CC" gate="G$1" x="-179.07" y="228.6" smashed="yes" rot="R90">
<attribute name="NAME" x="-180.086" y="234.188" size="1.778" layer="94" rot="R180"/>
</instance>
<instance part="CA" gate="G$1" x="-161.29" y="228.6" smashed="yes" rot="R90">
<attribute name="NAME" x="-162.052" y="234.442" size="1.778" layer="94" rot="R180"/>
</instance>
</instances>
<busses>
<bus name="SEG_A[0..6]">
<segment>
<wire x1="-210.82" y1="193.04" x2="-210.82" y2="162.56" width="0.762" layer="92"/>
<label x="-172.72" y="170.18" size="1.778" layer="95" rot="R90"/>
<wire x1="-210.82" y1="162.56" x2="-248.92" y2="162.56" width="0.762" layer="92"/>
<wire x1="-210.82" y1="193.04" x2="-170.18" y2="193.04" width="0.762" layer="92"/>
<wire x1="-170.18" y1="193.04" x2="-170.18" y2="162.56" width="0.762" layer="92"/>
</segment>
</bus>
<bus name="SEG_B[0..6]">
<segment>
<wire x1="-129.54" y1="233.68" x2="-129.54" y2="152.4" width="0.762" layer="92"/>
<wire x1="-129.54" y1="152.4" x2="-147.32" y2="152.4" width="0.762" layer="92"/>
<wire x1="-147.32" y1="152.4" x2="-147.32" y2="195.58" width="0.762" layer="92"/>
<label x="-132.08" y="149.86" size="1.778" layer="95" rot="R180"/>
</segment>
</bus>
<bus name="DIG[0..7]">
<segment>
<wire x1="-251.46" y1="203.2" x2="-137.16" y2="203.2" width="0.762" layer="92"/>
<wire x1="-137.16" y1="203.2" x2="-137.16" y2="238.76" width="0.762" layer="92"/>
<wire x1="-137.16" y1="238.76" x2="-124.46" y2="238.76" width="0.762" layer="92"/>
<wire x1="-124.46" y1="238.76" x2="-124.46" y2="149.86" width="0.762" layer="92"/>
<label x="-134.62" y="241.3" size="1.778" layer="95"/>
</segment>
</bus>
</busses>
<nets>
<net name="SEG_A0" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="B1"/>
<wire x1="-223.52" y1="185.42" x2="-223.52" y2="190.5" width="0.1524" layer="91"/>
<wire x1="-223.52" y1="190.5" x2="-215.9" y2="190.5" width="0.1524" layer="91"/>
<wire x1="-215.9" y1="190.5" x2="-210.82" y2="190.5" width="0.1524" layer="91"/>
<wire x1="-215.9" y1="190.5" x2="-215.9" y2="198.12" width="0.1524" layer="91"/>
<junction x="-215.9" y="190.5"/>
<wire x1="-215.9" y1="198.12" x2="-151.13" y2="198.12" width="0.1524" layer="91"/>
<wire x1="-151.13" y1="198.12" x2="-151.13" y2="226.06" width="0.1524" layer="91"/>
<pinref part="CA" gate="G$1" pin="P$1"/>
<wire x1="-161.29" y1="228.6" x2="-161.29" y2="226.06" width="0.1524" layer="91"/>
<wire x1="-161.29" y1="226.06" x2="-151.13" y2="226.06" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R1" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="190.5" x2="-170.18" y2="190.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A1" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D2"/>
<wire x1="-248.92" y1="165.1" x2="-248.92" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R2" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="187.96" x2="-170.18" y2="187.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A2" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D3"/>
<wire x1="-246.38" y1="165.1" x2="-246.38" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R3" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="185.42" x2="-170.18" y2="185.42" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A3" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D4"/>
<wire x1="-243.84" y1="165.1" x2="-243.84" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R4" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="182.88" x2="-170.18" y2="182.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A4" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D5"/>
<wire x1="-231.14" y1="165.1" x2="-231.14" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R5" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="180.34" x2="-170.18" y2="180.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A5" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D6"/>
<wire x1="-228.6" y1="165.1" x2="-228.6" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R6" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="177.8" x2="-170.18" y2="177.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_A6" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="D7"/>
<wire x1="-226.06" y1="165.1" x2="-226.06" y2="162.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R7" gate="G$1" pin="1"/>
<wire x1="-160.02" y1="175.26" x2="-170.18" y2="175.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B0" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="190.5" x2="-147.32" y2="190.5" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="A"/>
<wire x1="-119.38" y1="228.6" x2="-129.54" y2="228.6" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="A"/>
<wire x1="-119.38" y1="182.88" x2="-129.54" y2="182.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B1" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="187.96" x2="-147.32" y2="187.96" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="B"/>
<wire x1="-119.38" y1="218.44" x2="-129.54" y2="218.44" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="B"/>
<wire x1="-119.38" y1="172.72" x2="-129.54" y2="172.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B2" class="0">
<segment>
<pinref part="R3" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="185.42" x2="-147.32" y2="185.42" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="C"/>
<wire x1="-119.38" y1="210.82" x2="-129.54" y2="210.82" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="C"/>
<wire x1="-119.38" y1="165.1" x2="-129.54" y2="165.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B3" class="0">
<segment>
<pinref part="R4" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="182.88" x2="-147.32" y2="182.88" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="D"/>
<wire x1="-119.38" y1="205.74" x2="-129.54" y2="205.74" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="D"/>
<wire x1="-119.38" y1="160.02" x2="-129.54" y2="160.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B4" class="0">
<segment>
<pinref part="R5" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="180.34" x2="-147.32" y2="180.34" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="E"/>
<wire x1="-119.38" y1="203.2" x2="-129.54" y2="203.2" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="E"/>
<wire x1="-119.38" y1="157.48" x2="-129.54" y2="157.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B5" class="0">
<segment>
<pinref part="R6" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="177.8" x2="-147.32" y2="177.8" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="F"/>
<wire x1="-119.38" y1="226.06" x2="-129.54" y2="226.06" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="F"/>
<wire x1="-119.38" y1="180.34" x2="-129.54" y2="180.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SEG_B6" class="0">
<segment>
<pinref part="R7" gate="G$1" pin="2"/>
<wire x1="-151.13" y1="175.26" x2="-147.32" y2="175.26" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$3" gate="G$1" pin="G"/>
<wire x1="-119.38" y1="213.36" x2="-129.54" y2="213.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$4" gate="G$1" pin="G"/>
<wire x1="-119.38" y1="167.64" x2="-129.54" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIG0" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="DIG1"/>
<wire x1="-119.38" y1="231.14" x2="-124.46" y2="231.14" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="B5"/>
<wire x1="-233.68" y1="185.42" x2="-233.68" y2="195.58" width="0.1524" layer="91"/>
<wire x1="-233.68" y1="195.58" x2="-233.68" y2="203.2" width="0.1524" layer="91"/>
<wire x1="-233.68" y1="195.58" x2="-241.3" y2="195.58" width="0.1524" layer="91"/>
<junction x="-233.68" y="195.58"/>
<pinref part="SK2" gate="G$1" pin="6"/>
<wire x1="-241.3" y1="195.58" x2="-241.3" y2="223.52" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIG1" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="DIG2"/>
<wire x1="-119.38" y1="223.52" x2="-124.46" y2="223.52" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="C0"/>
<wire x1="-243.84" y1="185.42" x2="-243.84" y2="203.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIG2" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="DIG3"/>
<wire x1="-119.38" y1="220.98" x2="-124.46" y2="220.98" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="C1"/>
<wire x1="-246.38" y1="185.42" x2="-246.38" y2="203.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIG3" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="DIG4"/>
<wire x1="-119.38" y1="215.9" x2="-124.46" y2="215.9" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="C2"/>
<wire x1="-248.92" y1="185.42" x2="-248.92" y2="203.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="DIG4" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG1"/>
<wire x1="-119.38" y1="185.42" x2="-124.46" y2="185.42" width="0.1524" layer="91"/>
</segment>
<segment>
<wire x1="-218.44" y1="203.2" x2="-218.44" y2="165.1" width="0.1524" layer="91"/>
<wire x1="-218.44" y1="165.1" x2="-223.52" y2="165.1" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="B0"/>
</segment>
</net>
<net name="DIG5" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG2"/>
<wire x1="-119.38" y1="177.8" x2="-124.46" y2="177.8" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="B2"/>
<wire x1="-226.06" y1="185.42" x2="-226.06" y2="200.66" width="0.1524" layer="91"/>
<wire x1="-226.06" y1="200.66" x2="-226.06" y2="203.2" width="0.1524" layer="91"/>
<junction x="-226.06" y="200.66"/>
<wire x1="-184.15" y1="200.66" x2="-226.06" y2="200.66" width="0.1524" layer="91"/>
<wire x1="-184.15" y1="200.66" x2="-184.15" y2="228.6" width="0.1524" layer="91"/>
<pinref part="CC" gate="G$1" pin="P$2"/>
</segment>
</net>
<net name="DIG6" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG3"/>
<wire x1="-119.38" y1="175.26" x2="-124.46" y2="175.26" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="B3"/>
<wire x1="-228.6" y1="185.42" x2="-228.6" y2="200.66" width="0.1524" layer="91"/>
<pinref part="SK2" gate="G$1" pin="4"/>
<wire x1="-228.6" y1="200.66" x2="-228.6" y2="203.2" width="0.1524" layer="91"/>
<wire x1="-236.22" y1="223.52" x2="-236.22" y2="200.66" width="0.1524" layer="91"/>
<wire x1="-236.22" y1="200.66" x2="-228.6" y2="200.66" width="0.1524" layer="91"/>
<junction x="-228.6" y="200.66"/>
</segment>
</net>
<net name="DIG7" class="0">
<segment>
<pinref part="U$4" gate="G$1" pin="DIG4"/>
<wire x1="-119.38" y1="170.18" x2="-124.46" y2="170.18" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="B4"/>
<wire x1="-231.14" y1="185.42" x2="-231.14" y2="198.12" width="0.1524" layer="91"/>
<wire x1="-231.14" y1="198.12" x2="-231.14" y2="203.2" width="0.1524" layer="91"/>
<wire x1="-231.14" y1="198.12" x2="-238.76" y2="198.12" width="0.1524" layer="91"/>
<junction x="-231.14" y="198.12"/>
<pinref part="SK2" gate="G$1" pin="5"/>
<wire x1="-238.76" y1="198.12" x2="-238.76" y2="223.52" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="SK4" gate="G$1" pin="6"/>
<wire x1="-236.22" y1="137.16" x2="-236.22" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-236.22" y1="142.24" x2="-269.24" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-269.24" y1="142.24" x2="-269.24" y2="195.58" width="0.1524" layer="91"/>
<wire x1="-269.24" y1="195.58" x2="-251.46" y2="195.58" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C3"/>
<wire x1="-251.46" y1="195.58" x2="-251.46" y2="185.42" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="U$5" gate="G$1" pin="P$2"/>
<pinref part="R8" gate="G$1" pin="1"/>
<wire x1="-210.82" y1="215.9" x2="-215.9" y2="215.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U$5" gate="G$1" pin="P$3"/>
<pinref part="R9" gate="G$1" pin="1"/>
<wire x1="-210.82" y1="220.98" x2="-215.9" y2="220.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="R9" gate="G$1" pin="2"/>
<wire x1="-224.79" y1="220.98" x2="-228.6" y2="220.98" width="0.1524" layer="91"/>
<pinref part="SK2" gate="G$1" pin="1"/>
<wire x1="-228.6" y1="220.98" x2="-228.6" y2="223.52" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="SK2" gate="G$1" pin="2"/>
<wire x1="-231.14" y1="223.52" x2="-231.14" y2="215.9" width="0.1524" layer="91"/>
<pinref part="R8" gate="G$1" pin="2"/>
<wire x1="-231.14" y1="215.9" x2="-224.79" y2="215.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="0V" class="0">
<segment>
<pinref part="U$5" gate="G$1" pin="P$1"/>
<wire x1="-203.2" y1="218.44" x2="-198.12" y2="218.44" width="0.1524" layer="91"/>
<pinref part="GND1" gate="1" pin="0V"/>
<wire x1="-198.12" y1="218.44" x2="-198.12" y2="215.9" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="U$2" gate="G$1" pin="_0V"/>
<wire x1="-241.3" y1="185.42" x2="-241.3" y2="190.5" width="0.1524" layer="91"/>
<pinref part="GND2" gate="1" pin="0V"/>
<wire x1="-241.3" y1="190.5" x2="-276.86" y2="190.5" width="0.1524" layer="91"/>
<wire x1="-276.86" y1="190.5" x2="-276.86" y2="154.94" width="0.1524" layer="91"/>
<wire x1="-276.86" y1="154.94" x2="-276.86" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-238.76" y1="154.94" x2="-276.86" y2="154.94" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="0V"/>
<wire x1="-238.76" y1="165.1" x2="-238.76" y2="154.94" width="0.1524" layer="91"/>
<junction x="-276.86" y="154.94"/>
</segment>
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
<wire x1="-167.64" y1="254" x2="-167.64" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-167.64" y1="248.92" x2="-154.94" y2="248.92" width="0.1524" layer="91"/>
<pinref part="SW3" gate="G$1" pin="P$2"/>
<wire x1="-154.94" y1="248.92" x2="-147.32" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-147.32" y1="248.92" x2="-132.08" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-132.08" y1="248.92" x2="-121.92" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-121.92" y1="248.92" x2="-106.68" y2="248.92" width="0.1524" layer="91"/>
<wire x1="-106.68" y1="248.92" x2="-106.68" y2="254" width="0.1524" layer="91"/>
<pinref part="C7" gate="G$1" pin="P$1"/>
<wire x1="-121.92" y1="254" x2="-121.92" y2="248.92" width="0.1524" layer="91"/>
<junction x="-121.92" y="248.92"/>
<pinref part="SW2" gate="G$1" pin="P$2"/>
<wire x1="-132.08" y1="254" x2="-132.08" y2="248.92" width="0.1524" layer="91"/>
<junction x="-132.08" y="248.92"/>
<pinref part="C6" gate="G$1" pin="P$1"/>
<wire x1="-147.32" y1="254" x2="-147.32" y2="248.92" width="0.1524" layer="91"/>
<junction x="-147.32" y="248.92"/>
<pinref part="SW1" gate="G$1" pin="P$2"/>
<wire x1="-154.94" y1="254" x2="-154.94" y2="248.92" width="0.1524" layer="91"/>
<junction x="-154.94" y="248.92"/>
<pinref part="GND4" gate="1" pin="0V"/>
<wire x1="-106.68" y1="246.38" x2="-106.68" y2="248.92" width="0.1524" layer="91"/>
<junction x="-106.68" y="248.92"/>
</segment>
<segment>
<pinref part="SW4" gate="G$1" pin="P$1"/>
<pinref part="GND5" gate="1" pin="0V"/>
<wire x1="-187.96" y1="175.26" x2="-187.96" y2="180.34" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND6" gate="1" pin="0V"/>
<pinref part="C1" gate="G$1" pin="P$1"/>
<wire x1="-152.4" y1="127" x2="-152.4" y2="129.54" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="P$1"/>
<pinref part="GND7" gate="1" pin="0V"/>
<wire x1="-233.68" y1="246.38" x2="-233.68" y2="251.46" width="0.1524" layer="91"/>
<pinref part="C3" gate="G$1" pin="P$1"/>
<wire x1="-233.68" y1="251.46" x2="-233.68" y2="256.54" width="0.1524" layer="91"/>
<wire x1="-220.98" y1="256.54" x2="-220.98" y2="251.46" width="0.1524" layer="91"/>
<wire x1="-220.98" y1="251.46" x2="-233.68" y2="251.46" width="0.1524" layer="91"/>
<junction x="-233.68" y="251.46"/>
</segment>
<segment>
<pinref part="C4" gate="G$1" pin="P$1"/>
<pinref part="GND8" gate="1" pin="0V"/>
</segment>
</net>
<net name="+5V" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="5V"/>
<wire x1="-241.3" y1="165.1" x2="-241.3" y2="157.48" width="0.1524" layer="91"/>
<wire x1="-241.3" y1="157.48" x2="-264.16" y2="157.48" width="0.1524" layer="91"/>
<wire x1="-264.16" y1="157.48" x2="-264.16" y2="193.04" width="0.1524" layer="91"/>
<pinref part="P+1" gate="1" pin="+5V"/>
<pinref part="U$2" gate="G$1" pin="_5V"/>
<wire x1="-264.16" y1="193.04" x2="-264.16" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-236.22" y1="185.42" x2="-236.22" y2="193.04" width="0.1524" layer="91"/>
<wire x1="-236.22" y1="193.04" x2="-264.16" y2="193.04" width="0.1524" layer="91"/>
<junction x="-264.16" y="193.04"/>
</segment>
<segment>
<pinref part="SK4" gate="G$1" pin="4"/>
<pinref part="P+3" gate="1" pin="+5V"/>
<wire x1="-231.14" y1="137.16" x2="-231.14" y2="147.32" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R13" gate="G$1" pin="2"/>
<pinref part="P+4" gate="1" pin="+5V"/>
<wire x1="-200.66" y1="179.07" x2="-200.66" y2="185.42" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="R16" gate="G$1" pin="2"/>
<pinref part="P+5" gate="1" pin="+5V"/>
<wire x1="-140.97" y1="139.7" x2="-137.16" y2="139.7" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="P$2"/>
<pinref part="P+6" gate="1" pin="+5V"/>
<wire x1="-233.68" y1="269.24" x2="-233.68" y2="266.7" width="0.1524" layer="91"/>
<pinref part="C3" gate="G$1" pin="P$2"/>
<wire x1="-233.68" y1="266.7" x2="-233.68" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-220.98" y1="264.16" x2="-220.98" y2="266.7" width="0.1524" layer="91"/>
<wire x1="-220.98" y1="266.7" x2="-233.68" y2="266.7" width="0.1524" layer="91"/>
<junction x="-233.68" y="266.7"/>
</segment>
<segment>
<pinref part="P+7" gate="1" pin="+5V"/>
<pinref part="C4" gate="G$1" pin="P$2"/>
<wire x1="-251.46" y1="144.78" x2="-251.46" y2="139.7" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="SK3" gate="G$1" pin="1"/>
<wire x1="-175.26" y1="137.16" x2="-175.26" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-175.26" y1="142.24" x2="-170.18" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-170.18" y1="142.24" x2="-170.18" y2="137.16" width="0.1524" layer="91"/>
<wire x1="-170.18" y1="137.16" x2="-170.18" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-170.18" y1="124.46" x2="-284.48" y2="124.46" width="0.1524" layer="91"/>
<wire x1="-284.48" y1="124.46" x2="-284.48" y2="185.42" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C5"/>
<wire x1="-284.48" y1="185.42" x2="-256.54" y2="185.42" width="0.1524" layer="91"/>
<pinref part="R15" gate="G$1" pin="1"/>
<wire x1="-165.1" y1="137.16" x2="-170.18" y2="137.16" width="0.1524" layer="91"/>
<junction x="-170.18" y="137.16"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="C4"/>
<wire x1="-254" y1="185.42" x2="-254" y2="187.96" width="0.1524" layer="91"/>
<wire x1="-254" y1="187.96" x2="-289.56" y2="187.96" width="0.1524" layer="91"/>
<wire x1="-289.56" y1="187.96" x2="-289.56" y2="119.38" width="0.1524" layer="91"/>
<wire x1="-289.56" y1="119.38" x2="-167.64" y2="119.38" width="0.1524" layer="91"/>
<wire x1="-167.64" y1="119.38" x2="-167.64" y2="142.24" width="0.1524" layer="91"/>
<wire x1="-167.64" y1="142.24" x2="-167.64" y2="144.78" width="0.1524" layer="91"/>
<wire x1="-167.64" y1="144.78" x2="-177.8" y2="144.78" width="0.1524" layer="91"/>
<pinref part="SK3" gate="G$1" pin="2"/>
<wire x1="-177.8" y1="144.78" x2="-177.8" y2="137.16" width="0.1524" layer="91"/>
<pinref part="R14" gate="G$1" pin="1"/>
<wire x1="-165.1" y1="142.24" x2="-167.64" y2="142.24" width="0.1524" layer="91"/>
<junction x="-167.64" y="142.24"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<wire x1="-254" y1="162.56" x2="-281.94" y2="162.56" width="0.1524" layer="91"/>
<wire x1="-281.94" y1="162.56" x2="-281.94" y2="238.76" width="0.1524" layer="91"/>
<wire x1="-281.94" y1="238.76" x2="-143.51" y2="238.76" width="0.1524" layer="91"/>
<wire x1="-143.51" y1="238.76" x2="-143.51" y2="210.82" width="0.1524" layer="91"/>
<wire x1="-143.51" y1="210.82" x2="-157.48" y2="210.82" width="0.1524" layer="91"/>
<pinref part="SK1" gate="G$1" pin="1"/>
<wire x1="-157.48" y1="210.82" x2="-157.48" y2="213.36" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="D0"/>
<wire x1="-254" y1="165.1" x2="-254" y2="162.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<wire x1="-274.32" y1="160.02" x2="-274.32" y2="236.22" width="0.1524" layer="91"/>
<wire x1="-274.32" y1="236.22" x2="-146.05" y2="236.22" width="0.1524" layer="91"/>
<wire x1="-146.05" y1="236.22" x2="-146.05" y2="205.74" width="0.1524" layer="91"/>
<wire x1="-146.05" y1="205.74" x2="-160.02" y2="205.74" width="0.1524" layer="91"/>
<pinref part="SK1" gate="G$1" pin="2"/>
<wire x1="-160.02" y1="205.74" x2="-160.02" y2="213.36" width="0.1524" layer="91"/>
<wire x1="-251.46" y1="160.02" x2="-274.32" y2="160.02" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="D1"/>
<wire x1="-251.46" y1="160.02" x2="-251.46" y2="165.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="SW1" gate="G$1" pin="P$1"/>
<wire x1="-154.94" y1="259.08" x2="-154.94" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R10" gate="G$1" pin="2"/>
<wire x1="-154.94" y1="264.16" x2="-156.21" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="SW2" gate="G$1" pin="P$1"/>
<wire x1="-132.08" y1="259.08" x2="-132.08" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R11" gate="G$1" pin="2"/>
<wire x1="-132.08" y1="264.16" x2="-135.89" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="SW3" gate="G$1" pin="P$1"/>
<wire x1="-106.68" y1="259.08" x2="-106.68" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R12" gate="G$1" pin="2"/>
<wire x1="-106.68" y1="264.16" x2="-110.49" y2="264.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="C7" gate="G$1" pin="P$2"/>
<wire x1="-121.92" y1="261.62" x2="-121.92" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R12" gate="G$1" pin="1"/>
<wire x1="-121.92" y1="264.16" x2="-119.38" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-121.92" y1="264.16" x2="-121.92" y2="274.32" width="0.1524" layer="91"/>
<junction x="-121.92" y="264.16"/>
<wire x1="-121.92" y1="274.32" x2="-191.77" y2="274.32" width="0.1524" layer="91"/>
<wire x1="-191.77" y1="274.32" x2="-191.77" y2="207.01" width="0.1524" layer="91"/>
<wire x1="-191.77" y1="207.01" x2="-162.56" y2="207.01" width="0.1524" layer="91"/>
<pinref part="SK1" gate="G$1" pin="3"/>
<wire x1="-162.56" y1="207.01" x2="-162.56" y2="213.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="C5" gate="G$1" pin="P$2"/>
<wire x1="-167.64" y1="261.62" x2="-167.64" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R10" gate="G$1" pin="1"/>
<wire x1="-167.64" y1="264.16" x2="-165.1" y2="264.16" width="0.1524" layer="91"/>
<pinref part="SK2" gate="G$1" pin="3"/>
<wire x1="-233.68" y1="223.52" x2="-233.68" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-233.68" y1="208.28" x2="-194.31" y2="208.28" width="0.1524" layer="91"/>
<wire x1="-194.31" y1="208.28" x2="-194.31" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-194.31" y1="264.16" x2="-167.64" y2="264.16" width="0.1524" layer="91"/>
<junction x="-167.64" y="264.16"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="SK1" gate="G$1" pin="8"/>
<wire x1="-175.26" y1="213.36" x2="-175.26" y2="209.55" width="0.1524" layer="91"/>
<wire x1="-175.26" y1="209.55" x2="-189.23" y2="209.55" width="0.1524" layer="91"/>
<wire x1="-189.23" y1="209.55" x2="-189.23" y2="271.78" width="0.1524" layer="91"/>
<wire x1="-189.23" y1="271.78" x2="-147.32" y2="271.78" width="0.1524" layer="91"/>
<pinref part="C6" gate="G$1" pin="P$2"/>
<wire x1="-147.32" y1="261.62" x2="-147.32" y2="264.16" width="0.1524" layer="91"/>
<pinref part="R11" gate="G$1" pin="1"/>
<wire x1="-147.32" y1="264.16" x2="-144.78" y2="264.16" width="0.1524" layer="91"/>
<wire x1="-147.32" y1="271.78" x2="-147.32" y2="264.16" width="0.1524" layer="91"/>
<junction x="-147.32" y="264.16"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="B7"/>
<wire x1="-233.68" y1="165.1" x2="-233.68" y2="158.75" width="0.1524" layer="91"/>
<pinref part="U$16" gate="G$1" pin="P$1"/>
<wire x1="-233.68" y1="158.75" x2="-231.14" y2="158.75" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="U$16" gate="G$1" pin="P$2"/>
<wire x1="-231.14" y1="157.48" x2="-236.22" y2="157.48" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="B6"/>
<wire x1="-236.22" y1="157.48" x2="-236.22" y2="165.1" width="0.1524" layer="91"/>
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
<net name="N$20" class="0">
<segment>
<pinref part="R13" gate="G$1" pin="1"/>
<wire x1="-200.66" y1="170.18" x2="-200.66" y2="152.4" width="0.1524" layer="91"/>
<pinref part="SK3" gate="G$1" pin="3"/>
<wire x1="-180.34" y1="137.16" x2="-180.34" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-180.34" y1="152.4" x2="-187.96" y2="152.4" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C6"/>
<wire x1="-187.96" y1="152.4" x2="-200.66" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-200.66" y1="152.4" x2="-256.54" y2="152.4" width="0.1524" layer="91"/>
<wire x1="-256.54" y1="152.4" x2="-256.54" y2="165.1" width="0.1524" layer="91"/>
<junction x="-200.66" y="152.4"/>
<pinref part="R17" gate="G$1" pin="1"/>
<wire x1="-187.96" y1="157.48" x2="-187.96" y2="152.4" width="0.1524" layer="91"/>
<junction x="-187.96" y="152.4"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="R17" gate="G$1" pin="2"/>
<pinref part="SW4" gate="G$1" pin="P$2"/>
<wire x1="-187.96" y1="166.37" x2="-187.96" y2="170.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="CC" gate="G$1" pin="P$1"/>
<wire x1="-179.07" y1="228.6" x2="-179.07" y2="226.06" width="0.1524" layer="91"/>
<wire x1="-179.07" y1="226.06" x2="-166.37" y2="226.06" width="0.1524" layer="91"/>
<pinref part="CA" gate="G$1" pin="P$2"/>
<wire x1="-166.37" y1="226.06" x2="-166.37" y2="228.6" width="0.1524" layer="91"/>
<pinref part="SK1" gate="G$1" pin="7"/>
<wire x1="-172.72" y1="213.36" x2="-172.72" y2="212.09" width="0.1524" layer="91"/>
<wire x1="-172.72" y1="212.09" x2="-179.07" y2="212.09" width="0.1524" layer="91"/>
<wire x1="-179.07" y1="212.09" x2="-179.07" y2="226.06" width="0.1524" layer="91"/>
<junction x="-179.07" y="226.06"/>
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
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
