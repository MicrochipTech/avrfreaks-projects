<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.4">
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
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="no" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="piher">
<description>&lt;b&gt;Piher Potentiometers&lt;/b&gt;&lt;p&gt;
PC-16/16 mm carbon types&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="PC16S">
<description>&lt;b&gt;POTENTIOMETER&lt;/b&gt;&lt;p&gt;
single carbon potentiometer, 16 mm</description>
<wire x1="-7.9" y1="0.1" x2="-4.9" y2="0.1" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="0.1" x2="4.9" y2="0.1" width="0.2032" layer="21"/>
<wire x1="4.9" y1="0.1" x2="7.9" y2="0.1" width="0.2032" layer="21"/>
<wire x1="-7.9" y1="8" x2="7.9" y2="8" width="0.2032" layer="21"/>
<wire x1="-4.5" y1="-8.8" x2="4.5" y2="-8.8" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="0.1" x2="-4.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="-3.7" x2="-4.9" y2="-8.4" width="0.2032" layer="21"/>
<wire x1="4.9" y1="0.1" x2="4.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="4.9" y1="-3.7" x2="4.9" y2="-8.4" width="0.2032" layer="21"/>
<wire x1="-7.9" y1="8" x2="-7.9" y2="0.1" width="0.2032" layer="21"/>
<wire x1="7.9" y1="0.1" x2="7.9" y2="8" width="0.2032" layer="21"/>
<wire x1="-2.9" y1="-44.9" x2="2.9" y2="-44.9" width="0.2032" layer="21"/>
<wire x1="-2.9" y1="-8.9" x2="-2.9" y2="-44.9" width="0.2032" layer="21"/>
<wire x1="2.9" y1="-8.9" x2="2.9" y2="-44.9" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="-8.4" x2="-4.5" y2="-8.8" width="0.2032" layer="21"/>
<wire x1="4.5" y1="-8.8" x2="4.9" y2="-8.4" width="0.2032" layer="21"/>
<wire x1="-5.7" y1="-1.9" x2="-4.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="-1.9" x2="4.9" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="4.9" y1="-1.9" x2="5.7" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="5.7" y1="-1.9" x2="6.4" y2="-2" width="0.2032" layer="21"/>
<wire x1="6.4" y1="-2" x2="6.7" y2="-2.1" width="0.2032" layer="21"/>
<wire x1="6.7" y1="-2.1" x2="6.7" y2="-3.5" width="0.2032" layer="21"/>
<wire x1="6.7" y1="-3.5" x2="6.4" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="6.4" y1="-3.6" x2="5.7" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="5.7" y1="-3.7" x2="4.9" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="4.9" y1="-3.7" x2="-4.9" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="-4.9" y1="-3.7" x2="-5.7" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="-5.7" y1="-3.7" x2="-6.4" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="-6.4" y1="-3.6" x2="-6.7" y2="-3.5" width="0.2032" layer="21"/>
<wire x1="-6.7" y1="-3.5" x2="-6.7" y2="-2.1" width="0.2032" layer="21"/>
<wire x1="-6.7" y1="-2.1" x2="-6.4" y2="-2" width="0.2032" layer="21"/>
<wire x1="-6.4" y1="-2" x2="-5.7" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="3.4" y1="-2.4" x2="3" y2="-2.2" width="0.2032" layer="21"/>
<wire x1="3" y1="-2.2" x2="2.2" y2="-2" width="0.2032" layer="21"/>
<wire x1="2.2" y1="-2" x2="1.4" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="-3.4" y1="-2.4" x2="-3" y2="-2.2" width="0.2032" layer="21"/>
<wire x1="-3" y1="-2.2" x2="-2.2" y2="-2" width="0.2032" layer="21"/>
<wire x1="-2.2" y1="-2" x2="-1.4" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="-3" y1="-3.4" x2="-2.2" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="-2.2" y1="-3.6" x2="-1.4" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="3.4" y1="-3.2" x2="3" y2="-3.4" width="0.2032" layer="21"/>
<wire x1="3" y1="-3.4" x2="2.2" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="2.2" y1="-3.6" x2="1.4" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="-3.4" y1="-3.2" x2="-3.8" y2="-3.4" width="0.2032" layer="21"/>
<wire x1="-3.8" y1="-3.4" x2="-4.6" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="-4.6" y1="-3.6" x2="-5.4" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="-3.4" y1="-2.4" x2="-3.8" y2="-2.2" width="0.2032" layer="21"/>
<wire x1="-3.8" y1="-2.2" x2="-4.6" y2="-2" width="0.2032" layer="21"/>
<wire x1="-4.6" y1="-2" x2="-5.4" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="3.4" y1="-2.4" x2="3.8" y2="-2.2" width="0.2032" layer="21"/>
<wire x1="3.8" y1="-2.2" x2="4.6" y2="-2" width="0.2032" layer="21"/>
<wire x1="4.6" y1="-2" x2="5.4" y2="-1.9" width="0.2032" layer="21"/>
<wire x1="3.4" y1="-3.2" x2="3.8" y2="-3.4" width="0.2032" layer="21"/>
<wire x1="3.8" y1="-3.4" x2="4.6" y2="-3.6" width="0.2032" layer="21"/>
<wire x1="4.6" y1="-3.6" x2="5.4" y2="-3.7" width="0.2032" layer="21"/>
<wire x1="-3.4" y1="-2.4" x2="-3.4" y2="-3.2" width="0.2032" layer="21"/>
<wire x1="3.4" y1="-2.4" x2="3.4" y2="-3.2" width="0.2032" layer="21"/>
<wire x1="-3.4072" y1="-3.2" x2="-3.0072" y2="-3.4" width="0.2032" layer="21"/>
<pad name="1A" x="-5" y="6.5" drill="1.2" shape="long" rot="R90"/>
<pad name="1S" x="0" y="6.5" drill="1.2" shape="long" rot="R90"/>
<pad name="1E" x="5" y="6.5" drill="1.2" shape="long" rot="R90"/>
<text x="-8.32" y="0.52" size="1.27" layer="25" rot="R90">&gt;NAME</text>
<text x="9.9" y="0.52" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="POT">
<wire x1="-4.318" y1="-1.524" x2="-4.318" y2="-0.508" width="0.254" layer="94"/>
<wire x1="-4.318" y1="-0.508" x2="-4.318" y2="0.508" width="0.254" layer="94"/>
<wire x1="-4.318" y1="0.508" x2="4.318" y2="0.508" width="0.254" layer="94"/>
<wire x1="4.318" y1="0.508" x2="4.318" y2="-0.508" width="0.254" layer="94"/>
<wire x1="4.318" y1="-0.508" x2="4.318" y2="-1.524" width="0.254" layer="94"/>
<wire x1="4.318" y1="-1.524" x2="-4.318" y2="-1.524" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-2.54" x2="-5.08" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="-5.08" y1="-0.508" x2="-4.318" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-2.54" x2="5.08" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-0.508" x2="4.318" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="-3.81" y2="1.778" width="0.1524" layer="94"/>
<wire x1="-4.318" y1="1.27" x2="-3.81" y2="1.778" width="0.1524" layer="94"/>
<wire x1="-3.81" y1="1.778" x2="-3.302" y2="2.286" width="0.1524" layer="94"/>
<text x="-4.064" y="3.556" size="1.778" layer="95">&gt;NAME</text>
<text x="-2.286" y="1.27" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1A" x="-5.08" y="-5.08" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="1S" x="0" y="-5.08" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="1E" x="5.08" y="-5.08" visible="pad" length="short" direction="pas" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PC16S" prefix="R">
<description>&lt;b&gt;POTENTIOMETER&lt;/b&gt;&lt;p&gt;
single carbon potentiometer, 16 mm, S3ROH</description>
<gates>
<gate name="G$1" symbol="POT" x="0" y="0"/>
</gates>
<devices>
<device name="" package="PC16S">
<connects>
<connect gate="G$1" pin="1A" pad="1A"/>
<connect gate="G$1" pin="1E" pad="1E"/>
<connect gate="G$1" pin="1S" pad="1S"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="con-lsta">
<description>&lt;b&gt;Female Headers etc.&lt;/b&gt;&lt;p&gt;
Naming:&lt;p&gt;
FE = female&lt;p&gt;
# contacts - # rows&lt;p&gt;
W = angled&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="FE05-2">
<description>&lt;b&gt;FEMALE HEADER&lt;/b&gt;</description>
<wire x1="-5.715" y1="3.048" x2="5.715" y2="3.048" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-2.413" x2="6.35" y2="2.413" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="-2.413" x2="-6.35" y2="2.413" width="0.1524" layer="21"/>
<wire x1="-5.715" y1="-3.048" x2="5.715" y2="-3.048" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="2.413" x2="-5.715" y2="3.048" width="0.1524" layer="21" curve="-90"/>
<wire x1="5.715" y1="3.048" x2="6.35" y2="2.413" width="0.1524" layer="21" curve="-90"/>
<wire x1="5.715" y1="-3.048" x2="6.35" y2="-2.413" width="0.1524" layer="21" curve="90"/>
<wire x1="-6.35" y1="-2.413" x2="-5.715" y2="-3.048" width="0.1524" layer="21" curve="90"/>
<circle x="-5.08" y="-1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="-5.08" y="1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="-2.54" y="-1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="-2.54" y="1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="0" y="-1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="0" y="1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="2.54" y="-1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="2.54" y="1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="5.08" y="-1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="5.08" y="1.27" radius="0.127" width="0.4064" layer="51"/>
<circle x="-5.08" y="-1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="-5.08" y="1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="-2.54" y="1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="0" y="1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="2.54" y="1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="5.08" y="1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="-2.54" y="-1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="0" y="-1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="2.54" y="-1.27" radius="0.889" width="0.1524" layer="51"/>
<circle x="5.08" y="-1.27" radius="0.889" width="0.1524" layer="51"/>
<pad name="1" x="-5.08" y="1.27" drill="0.9144"/>
<pad name="2" x="-5.08" y="-1.27" drill="0.9144" shape="octagon"/>
<pad name="3" x="-2.54" y="1.27" drill="0.9144" shape="octagon"/>
<pad name="4" x="-2.54" y="-1.27" drill="0.9144" shape="octagon"/>
<pad name="5" x="0" y="1.27" drill="0.9144" shape="octagon"/>
<pad name="6" x="0" y="-1.27" drill="0.9144" shape="octagon"/>
<pad name="7" x="2.54" y="1.27" drill="0.9144" shape="octagon"/>
<pad name="8" x="2.54" y="-1.27" drill="0.9144" shape="octagon"/>
<pad name="9" x="5.08" y="1.27" drill="0.9144" shape="octagon"/>
<pad name="10" x="5.08" y="-1.27" drill="0.9144" shape="octagon"/>
<text x="-2.54" y="3.429" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-5.461" y="3.429" size="1.27" layer="21" ratio="10">1</text>
<text x="-6.35" y="-4.699" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="4.191" y="-4.699" size="1.27" layer="21" ratio="10">10</text>
</package>
</packages>
<symbols>
<symbol name="FE05-2">
<wire x1="3.81" y1="-7.62" x2="-3.81" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="-1.905" y1="-0.635" x2="-1.905" y2="0.635" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="-1.905" y1="-3.175" x2="-1.905" y2="-1.905" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="-1.905" y1="-5.715" x2="-1.905" y2="-4.445" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="1.905" y1="0.635" x2="1.905" y2="-0.635" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="1.905" y1="-1.905" x2="1.905" y2="-3.175" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="1.905" y1="-4.445" x2="1.905" y2="-5.715" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="-3.81" y1="7.62" x2="-3.81" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-1.905" y1="4.445" x2="-1.905" y2="5.715" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="-1.905" y1="1.905" x2="-1.905" y2="3.175" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="1.905" y1="5.715" x2="1.905" y2="4.445" width="0.254" layer="94" curve="-180" cap="flat"/>
<wire x1="1.905" y1="3.175" x2="1.905" y2="1.905" width="0.254" layer="94" curve="-180" cap="flat"/>
<text x="-3.81" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-3.81" y="8.382" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="-7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="3" x="-7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="5" x="-7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="2" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="4" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="6" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="7" x="-7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="9" x="-7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="8" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="10" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="FE05-2" prefix="SV" uservalue="yes">
<description>&lt;b&gt;FEMALE HEADER&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="FE05-2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="FE05-2">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="con-lstb">
<description>&lt;b&gt;Pin Headers&lt;/b&gt;&lt;p&gt;
Naming:&lt;p&gt;
MA = male&lt;p&gt;
# contacts - # rows&lt;p&gt;
W = angled&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="MA05-2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-5.715" y1="2.54" x2="-4.445" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="2.54" x2="-3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="1.905" x2="-3.175" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="2.54" x2="-1.905" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="2.54" x2="-1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-5.715" y1="2.54" x2="-6.35" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="1.905" x2="-0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="2.54" x2="0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="2.54" x2="1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="1.905" y1="2.54" x2="3.175" y2="2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="2.54" x2="3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="1.905" x2="4.445" y2="2.54" width="0.1524" layer="21"/>
<wire x1="4.445" y1="2.54" x2="5.715" y2="2.54" width="0.1524" layer="21"/>
<wire x1="1.905" y1="2.54" x2="1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-1.905" x2="-4.445" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="-1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-2.54" x2="-3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-2.54" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="1.905" x2="-6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="-1.905" x2="-5.715" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="-2.54" x2="-5.715" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-2.54" x2="-0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-2.54" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="5.715" y1="-2.54" x2="4.445" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="4.445" y1="-2.54" x2="3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-2.54" x2="1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="6.35" y1="1.905" x2="6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="5.715" y1="2.54" x2="6.35" y2="1.905" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-1.905" x2="5.715" y2="-2.54" width="0.1524" layer="21"/>
<pad name="1" x="-5.08" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="3" x="-2.54" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="0" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="7" x="2.54" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="9" x="5.08" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="2" x="-5.08" y="1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="-2.54" y="1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="0" y="1.27" drill="1.016" shape="octagon"/>
<pad name="8" x="2.54" y="1.27" drill="1.016" shape="octagon"/>
<pad name="10" x="5.08" y="1.27" drill="1.016" shape="octagon"/>
<text x="-5.588" y="-4.191" size="1.27" layer="21" ratio="10">1</text>
<text x="-6.35" y="2.921" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="0" y="-4.191" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="4.572" y="2.921" size="1.27" layer="21" ratio="10">10</text>
<rectangle x1="-2.794" y1="-1.524" x2="-2.286" y2="-1.016" layer="51"/>
<rectangle x1="-5.334" y1="-1.524" x2="-4.826" y2="-1.016" layer="51"/>
<rectangle x1="-0.254" y1="-1.524" x2="0.254" y2="-1.016" layer="51"/>
<rectangle x1="4.826" y1="-1.524" x2="5.334" y2="-1.016" layer="51"/>
<rectangle x1="2.286" y1="-1.524" x2="2.794" y2="-1.016" layer="51"/>
<rectangle x1="-5.334" y1="1.016" x2="-4.826" y2="1.524" layer="51"/>
<rectangle x1="-2.794" y1="1.016" x2="-2.286" y2="1.524" layer="51"/>
<rectangle x1="-0.254" y1="1.016" x2="0.254" y2="1.524" layer="51"/>
<rectangle x1="2.286" y1="1.016" x2="2.794" y2="1.524" layer="51"/>
<rectangle x1="4.826" y1="1.016" x2="5.334" y2="1.524" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="MA05-2">
<wire x1="3.81" y1="-7.62" x2="-3.81" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="2.54" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.27" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-1.27" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-5.08" x2="-1.27" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-3.81" y1="7.62" x2="-3.81" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="5.08" x2="2.54" y2="5.08" width="0.6096" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="-1.27" y2="5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-1.27" y2="2.54" width="0.6096" layer="94"/>
<text x="-3.81" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-3.81" y="8.382" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="5" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="-7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="4" x="-7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="6" x="-7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="7" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="9" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="8" x="-7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="10" x="-7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="MA05-2" prefix="SV" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="MA05-2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MA05-2">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="switch-dil">
<description>&lt;b&gt;DIL Switches and Code Switches&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="SD4-01">
<description>&lt;b&gt;DIL/CODE SWITCH&lt;/b&gt;&lt;p&gt;
Mors</description>
<wire x1="2.032" y1="-3.175" x2="2.032" y2="3.175" width="0.254" layer="21"/>
<wire x1="-2.032" y1="3.175" x2="-2.032" y2="-3.175" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.254" layer="21"/>
<wire x1="0.635" y1="1.27" x2="-0.635" y2="1.27" width="0.254" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-0.635" y2="-1.27" width="0.254" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="0.635" y2="1.27" width="0.254" layer="21"/>
<wire x1="-2.032" y1="-3.175" x2="-0.762" y2="-3.175" width="0.254" layer="21"/>
<wire x1="2.032" y1="-3.175" x2="0.762" y2="-3.175" width="0.254" layer="21"/>
<wire x1="0.762" y1="-3.175" x2="-0.762" y2="-3.175" width="0.254" layer="51"/>
<wire x1="2.032" y1="3.175" x2="0.762" y2="3.175" width="0.254" layer="21"/>
<wire x1="-2.032" y1="3.175" x2="-0.762" y2="3.175" width="0.254" layer="21"/>
<wire x1="-0.762" y1="3.175" x2="0.762" y2="3.175" width="0.254" layer="51"/>
<wire x1="0.508" y1="-1.016" x2="-0.508" y2="-1.016" width="0.254" layer="21"/>
<wire x1="-0.508" y1="-0.254" x2="0.508" y2="-0.254" width="0.254" layer="21"/>
<pad name="1" x="0" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="0" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-0.381" y="-2.667" size="0.9906" layer="21" ratio="12">1</text>
<text x="-2.3622" y="-3.1242" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="3.6322" y="-3.1496" size="1.27" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<text x="-1.651" y="1.7018" size="0.9906" layer="21" ratio="12">ON</text>
<rectangle x1="-0.381" y1="-3.937" x2="0.381" y2="-3.175" layer="51"/>
<rectangle x1="-0.381" y1="3.175" x2="0.381" y2="3.937" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="DIP01">
<wire x1="-1.905" y1="5.08" x2="-1.905" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-1.905" y1="5.08" x2="1.905" y2="5.08" width="0.4064" layer="94"/>
<wire x1="1.905" y1="5.08" x2="1.905" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-1.905" y1="-5.08" x2="1.905" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="0.508" y1="-2.54" x2="-0.508" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-0.508" y1="2.54" x2="0.508" y2="2.54" width="0.1524" layer="94"/>
<wire x1="0.508" y1="2.54" x2="0.508" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-0.508" y1="-2.54" x2="-0.508" y2="2.54" width="0.1524" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="95" rot="R90">&gt;NAME</text>
<text x="4.445" y="-5.08" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<text x="-0.127" y="-4.064" size="0.9906" layer="94" ratio="14">1</text>
<text x="-0.889" y="3.175" size="0.9906" layer="94" ratio="14">ON</text>
<rectangle x1="-0.254" y1="-2.286" x2="0.254" y2="0" layer="94"/>
<pin name="2" x="0" y="7.62" visible="pad" length="short" direction="pas" rot="R270"/>
<pin name="1" x="0" y="-7.62" visible="pad" length="short" direction="pas" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="SD4-01" prefix="S" uservalue="yes">
<description>&lt;b&gt;DIL/CODE SWITCH&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="DIP01" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SD4-01">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="battery">
<description>&lt;b&gt;Lithium Batteries and NC Accus&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="CR2032H">
<description>&lt;b&gt;LI BATTERY&lt;/b&gt; Varta</description>
<wire x1="-0.635" y1="8.255" x2="0.635" y2="8.255" width="0.254" layer="21"/>
<wire x1="0" y1="8.89" x2="0" y2="7.62" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-8.89" x2="0.635" y2="-8.89" width="0.254" layer="21"/>
<wire x1="-3.556" y1="11.049" x2="-6.604" y2="11.049" width="0.1524" layer="51"/>
<wire x1="-6.604" y1="11.049" x2="-6.604" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="6.604" y1="-2.54" x2="6.604" y2="11.049" width="0.1524" layer="21"/>
<wire x1="-6.604" y1="-2.54" x2="-1.27" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="3.175" x2="-1.27" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="3.175" x2="1.27" y2="3.175" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-2.54" x2="1.27" y2="3.175" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-2.54" x2="6.604" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-6.698" y1="7.5979" x2="10.033" y2="0" width="0.1524" layer="21" curve="227.397154" cap="flat"/>
<wire x1="3.556" y1="11.049" x2="-3.556" y2="11.049" width="0.1524" layer="21"/>
<wire x1="6.477" y1="11.049" x2="3.556" y2="11.049" width="0.1524" layer="51"/>
<wire x1="6.6203" y1="7.6668" x2="10.0331" y2="0" width="0.1524" layer="21" curve="-49.440271" cap="flat"/>
<pad name="+@1" x="-5.08" y="10.795" drill="1.1176" diameter="2.54" shape="octagon"/>
<pad name="+" x="5.08" y="10.795" drill="1.1176" diameter="2.54" shape="octagon"/>
<pad name="-" x="0" y="-6.985" drill="1.1176" diameter="2.54" shape="octagon"/>
<text x="-2.54" y="11.43" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="5.08" y="-1.905" size="1.27" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<text x="-2.921" y="-5.08" size="1.27" layer="21" ratio="10">Lit.3V</text>
</package>
</packages>
<symbols>
<symbol name="1V2+2">
<wire x1="-1.905" y1="0.635" x2="-1.905" y2="0" width="0.4064" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.905" y2="0" width="0.1524" layer="94"/>
<wire x1="-1.905" y1="0" x2="-1.905" y2="-0.635" width="0.4064" layer="94"/>
<wire x1="-0.635" y1="2.54" x2="-0.635" y2="0" width="0.4064" layer="94"/>
<wire x1="-0.635" y1="0" x2="2.54" y2="0" width="0.1524" layer="94"/>
<wire x1="-0.635" y1="0" x2="-0.635" y2="-2.54" width="0.4064" layer="94"/>
<text x="-2.54" y="3.175" size="1.778" layer="95">&gt;NAME</text>
<text x="-2.54" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<pin name="+" x="5.08" y="0" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="-" x="-5.08" y="0" visible="pad" length="short" direction="pas"/>
<pin name="+@1" x="2.54" y="0" visible="off" length="point" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="CR2032H" prefix="G">
<description>&lt;b&gt;LI BATTERY&lt;/b&gt; Varta</description>
<gates>
<gate name="1" symbol="1V2+2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="CR2032H">
<connects>
<connect gate="1" pin="+" pad="+"/>
<connect gate="1" pin="+@1" pad="+@1"/>
<connect gate="1" pin="-" pad="-"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="solpad">
<description>&lt;b&gt;Solder Pads/Test Points&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="LSP10">
<description>&lt;b&gt;SOLDER PAD&lt;/b&gt;&lt;p&gt;
drill 1.0 mm</description>
<wire x1="-1.27" y1="0.254" x2="-1.27" y2="-0.254" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.254" x2="1.27" y2="-0.254" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.254" x2="1.143" y2="0.254" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.254" x2="1.143" y2="-0.254" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-0.254" x2="-1.143" y2="-0.254" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.254" x2="-1.143" y2="0.254" width="0.1524" layer="21"/>
<wire x1="1.143" y1="0.254" x2="0.635" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-0.254" x2="-0.635" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.635" y1="0.254" x2="0.635" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.635" y1="0.254" x2="-0.635" y2="0.254" width="0.1524" layer="51"/>
<wire x1="0.635" y1="-0.254" x2="1.143" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="-0.635" y1="0.254" x2="-0.635" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="-0.635" y1="0.254" x2="-1.143" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.635" y1="-0.254" x2="0.635" y2="-0.254" width="0.1524" layer="51"/>
<pad name="MP" x="0" y="0" drill="1.016" diameter="2.159" shape="octagon"/>
<text x="-1.27" y="1.27" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="0" y="0.254" size="0.0254" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="LSP">
<wire x1="-1.016" y1="2.032" x2="1.016" y2="0" width="0.254" layer="94"/>
<wire x1="-1.016" y1="0" x2="1.016" y2="2.032" width="0.254" layer="94"/>
<circle x="0" y="1.016" radius="1.016" width="0.4064" layer="94"/>
<text x="-1.27" y="2.921" size="1.778" layer="95">&gt;NAME</text>
<pin name="MP" x="0" y="-2.54" visible="off" length="short" direction="pas" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="LSP10" prefix="LSP">
<description>&lt;b&gt;SOLDER PAD&lt;/b&gt;&lt;p&gt; drill 1.0 mm, distributor Buerklin, 12H555</description>
<gates>
<gate name="1" symbol="LSP" x="0" y="0"/>
</gates>
<devices>
<device name="" package="LSP10">
<connects>
<connect gate="1" pin="MP" pad="MP"/>
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
<part name="R" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R2" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R3" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R4" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R1" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R5" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R6" library="piher" deviceset="PC16S" device="" value=""/>
<part name="R7" library="piher" deviceset="PC16S" device="" value=""/>
<part name="SV1" library="con-lsta" deviceset="FE05-2" device=""/>
<part name="SV2" library="con-lstb" deviceset="MA05-2" device=""/>
<part name="S1" library="switch-dil" deviceset="SD4-01" device=""/>
<part name="G1" library="battery" deviceset="CR2032H" device="" value="2x AAA"/>
<part name="S2" library="switch-dil" deviceset="SD4-01" device=""/>
<part name="G2" library="battery" deviceset="CR2032H" device="" value="2x AAA"/>
<part name="LSP1" library="solpad" deviceset="LSP10" device=""/>
<part name="LSP2" library="solpad" deviceset="LSP10" device=""/>
<part name="LSP3" library="solpad" deviceset="LSP10" device=""/>
<part name="LSP4" library="solpad" deviceset="LSP10" device=""/>
<part name="LSP5" library="solpad" deviceset="LSP10" device=""/>
<part name="LSP6" library="solpad" deviceset="LSP10" device=""/>
<part name="SV3" library="con-lstb" deviceset="MA05-2" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="43.18" y="88.9" size="3.81" layer="97">Baby TX modification</text>
<circle x="27.94" y="58.42" radius="8.032184375" width="0.1524" layer="97"/>
<circle x="2.54" y="58.42" radius="8.032184375" width="0.1524" layer="97"/>
<text x="0" y="55.88" size="1.778" layer="97">Left
stick</text>
<text x="22.86" y="58.42" size="1.778" layer="97">Right stick</text>
<text x="2.54" y="78.74" size="2.54" layer="97">Before</text>
<text x="86.36" y="78.74" size="2.54" layer="97">After</text>
<rectangle x1="17.78" y1="30.48" x2="33.02" y2="40.64" layer="97"/>
<text x="17.78" y="27.94" size="2.54" layer="97">Controller</text>
<circle x="99.06" y="58.42" radius="8.032184375" width="0.1524" layer="97"/>
<circle x="73.66" y="58.42" radius="8.032184375" width="0.1524" layer="97"/>
<text x="71.12" y="55.88" size="1.778" layer="97">Left
stick</text>
<text x="93.98" y="58.42" size="1.778" layer="97">Right stick</text>
<rectangle x1="88.9" y1="12.7" x2="104.14" y2="22.86" layer="97"/>
<text x="88.9" y="10.16" size="2.54" layer="97">Controller</text>
<text x="151.13" y="30.988" size="1.778" layer="97">Connector
from Dongle</text>
<text x="107.696" y="29.718" size="1.778" layer="97">Baby TX
Plug 
modification</text>
<text x="135.382" y="37.592" size="1.778" layer="97">1</text>
<text x="91.948" y="38.1" size="1.778" layer="97">1</text>
<text x="139.7" y="0" size="1.778" layer="97">CH0</text>
<text x="142.24" y="5.08" size="1.778" layer="97">CH1</text>
<text x="144.78" y="10.16" size="1.778" layer="97">CH2</text>
<text x="147.32" y="15.24" size="1.778" layer="97">CH3</text>
<text x="149.352" y="21.59" size="1.778" layer="97">GND for
Baby TX</text>
<text x="149.86" y="45.72" size="1.778" layer="97">+3,3V for
Baby TX</text>
<wire x1="147.32" y1="43.18" x2="147.32" y2="40.64" width="0.1524" layer="97"/>
<text x="20.32" y="-5.08" size="1.778" layer="97">1</text>
<text x="-12.7" y="-7.62" size="1.778" layer="97">Dummy Plug for
standalone use of
Baby TX</text>
<wire x1="53.34" y1="86.36" x2="53.34" y2="10.16" width="0.1524" layer="97"/>
<wire x1="53.34" y1="10.16" x2="53.34" y2="-30.48" width="0.1524" layer="97"/>
<wire x1="53.34" y1="-30.48" x2="132.08" y2="-30.48" width="0.1524" layer="97"/>
<wire x1="132.08" y1="-30.48" x2="175.26" y2="-30.48" width="0.1524" layer="97"/>
<wire x1="175.26" y1="-30.48" x2="175.26" y2="86.36" width="0.1524" layer="97"/>
<wire x1="175.26" y1="86.36" x2="129.54" y2="86.36" width="0.1524" layer="97"/>
<wire x1="129.54" y1="86.36" x2="53.34" y2="86.36" width="0.1524" layer="97"/>
<wire x1="53.34" y1="86.36" x2="-17.78" y2="86.36" width="0.1524" layer="97"/>
<wire x1="-17.78" y1="86.36" x2="-17.78" y2="10.16" width="0.1524" layer="97"/>
<wire x1="-17.78" y1="10.16" x2="53.34" y2="10.16" width="0.1524" layer="97"/>
<wire x1="-17.78" y1="10.16" x2="-17.78" y2="-30.48" width="0.1524" layer="97"/>
<wire x1="-17.78" y1="-30.48" x2="53.34" y2="-30.48" width="0.1524" layer="97"/>
<wire x1="129.54" y1="86.36" x2="132.08" y2="86.36" width="0.1524" layer="97" style="longdash"/>
<wire x1="132.08" y1="86.36" x2="132.08" y2="-30.48" width="0.1524" layer="97" style="longdash"/>
</plain>
<instances>
<instance part="R" gate="G$1" x="-10.16" y="58.42" rot="R90"/>
<instance part="R2" gate="G$1" x="2.54" y="45.72" rot="R180"/>
<instance part="R3" gate="G$1" x="27.94" y="71.12"/>
<instance part="R4" gate="G$1" x="40.64" y="58.42" rot="R270"/>
<instance part="R1" gate="G$1" x="60.96" y="58.42" rot="R90"/>
<instance part="R5" gate="G$1" x="73.66" y="45.72" rot="R180"/>
<instance part="R6" gate="G$1" x="99.06" y="71.12"/>
<instance part="R7" gate="G$1" x="111.76" y="58.42" rot="R270"/>
<instance part="SV1" gate="G$1" x="99.06" y="33.02" rot="R270"/>
<instance part="SV2" gate="G$1" x="142.24" y="33.02" rot="R90"/>
<instance part="S1" gate="1" x="48.26" y="30.48" rot="R180"/>
<instance part="G1" gate="1" x="43.18" y="17.78"/>
<instance part="S2" gate="1" x="127" y="10.16" rot="R180"/>
<instance part="G2" gate="1" x="121.92" y="-2.54"/>
<instance part="LSP1" gate="1" x="137.16" y="2.54" rot="R180"/>
<instance part="LSP2" gate="1" x="139.7" y="7.62" rot="R180"/>
<instance part="LSP3" gate="1" x="142.24" y="12.7" rot="R180"/>
<instance part="LSP4" gate="1" x="144.78" y="17.78" rot="R180"/>
<instance part="LSP5" gate="1" x="147.32" y="22.86" rot="R180"/>
<instance part="LSP6" gate="1" x="147.32" y="45.72"/>
<instance part="SV3" gate="G$1" x="27.94" y="-10.16" rot="R90"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="R4" gate="G$1" pin="1S"/>
<wire x1="35.56" y1="58.42" x2="30.48" y2="58.42" width="0.1524" layer="91"/>
<wire x1="30.48" y1="58.42" x2="30.48" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="R3" gate="G$1" pin="1S"/>
<wire x1="27.94" y1="66.04" x2="27.94" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="1S"/>
<wire x1="2.54" y1="50.8" x2="2.54" y2="53.34" width="0.1524" layer="91"/>
<wire x1="2.54" y1="53.34" x2="22.86" y2="53.34" width="0.1524" layer="91"/>
<wire x1="22.86" y1="53.34" x2="22.86" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="R" gate="G$1" pin="1S"/>
<wire x1="-5.08" y1="58.42" x2="25.4" y2="58.42" width="0.1524" layer="91"/>
<wire x1="25.4" y1="58.42" x2="25.4" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="R7" gate="G$1" pin="1S"/>
<wire x1="106.68" y1="58.42" x2="101.6" y2="58.42" width="0.1524" layer="91"/>
<wire x1="101.6" y1="58.42" x2="101.6" y2="40.64" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="7"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="R6" gate="G$1" pin="1S"/>
<wire x1="99.06" y1="66.04" x2="99.06" y2="40.64" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="5"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="R5" gate="G$1" pin="1S"/>
<wire x1="73.66" y1="50.8" x2="73.66" y2="53.34" width="0.1524" layer="91"/>
<wire x1="73.66" y1="53.34" x2="93.98" y2="53.34" width="0.1524" layer="91"/>
<wire x1="93.98" y1="53.34" x2="93.98" y2="40.64" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="1"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="1S"/>
<wire x1="66.04" y1="58.42" x2="96.52" y2="58.42" width="0.1524" layer="91"/>
<wire x1="96.52" y1="58.42" x2="96.52" y2="40.64" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="3"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="SV1" gate="G$1" pin="2"/>
<wire x1="93.98" y1="25.4" x2="93.98" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="SV1" gate="G$1" pin="4"/>
<wire x1="96.52" y1="25.4" x2="96.52" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="SV1" gate="G$1" pin="6"/>
<wire x1="99.06" y1="25.4" x2="99.06" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="SV1" gate="G$1" pin="8"/>
<wire x1="101.6" y1="25.4" x2="101.6" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="G1" gate="1" pin="+"/>
<pinref part="S1" gate="1" pin="2"/>
<wire x1="48.26" y1="17.78" x2="48.26" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="G1" gate="1" pin="-"/>
<wire x1="38.1" y1="17.78" x2="35.56" y2="17.78" width="0.1524" layer="91"/>
<wire x1="35.56" y1="17.78" x2="35.56" y2="33.02" width="0.1524" layer="91"/>
<wire x1="35.56" y1="33.02" x2="33.02" y2="33.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="S1" gate="1" pin="1"/>
<wire x1="48.26" y1="38.1" x2="33.02" y2="38.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="G2" gate="1" pin="+"/>
<pinref part="S2" gate="1" pin="2"/>
<wire x1="127" y1="-2.54" x2="127" y2="2.54" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="G2" gate="1" pin="-"/>
<wire x1="116.84" y1="-2.54" x2="114.3" y2="-2.54" width="0.1524" layer="91"/>
<wire x1="114.3" y1="-2.54" x2="114.3" y2="15.24" width="0.1524" layer="91"/>
<wire x1="114.3" y1="15.24" x2="104.14" y2="15.24" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="10"/>
<wire x1="104.14" y1="25.4" x2="114.3" y2="25.4" width="0.1524" layer="91"/>
<wire x1="114.3" y1="25.4" x2="114.3" y2="15.24" width="0.1524" layer="91"/>
<junction x="114.3" y="15.24"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="S2" gate="1" pin="1"/>
<wire x1="127" y1="17.78" x2="127" y2="20.32" width="0.1524" layer="91"/>
<wire x1="127" y1="20.32" x2="104.14" y2="20.32" width="0.1524" layer="91"/>
<pinref part="SV1" gate="G$1" pin="9"/>
<wire x1="104.14" y1="40.64" x2="104.14" y2="43.18" width="0.1524" layer="91"/>
<wire x1="104.14" y1="43.18" x2="127" y2="43.18" width="0.1524" layer="91"/>
<wire x1="127" y1="43.18" x2="127" y2="20.32" width="0.1524" layer="91"/>
<junction x="127" y="20.32"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="LSP1" gate="1" pin="MP"/>
<pinref part="SV2" gate="G$1" pin="10"/>
<wire x1="137.16" y1="5.08" x2="137.16" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<pinref part="LSP2" gate="1" pin="MP"/>
<pinref part="SV2" gate="G$1" pin="8"/>
<wire x1="139.7" y1="10.16" x2="139.7" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="LSP3" gate="1" pin="MP"/>
<pinref part="SV2" gate="G$1" pin="6"/>
<wire x1="142.24" y1="15.24" x2="142.24" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="LSP4" gate="1" pin="MP"/>
<pinref part="SV2" gate="G$1" pin="4"/>
<wire x1="144.78" y1="20.32" x2="144.78" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="SV2" gate="G$1" pin="2"/>
<pinref part="LSP5" gate="1" pin="MP"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="SV3" gate="G$1" pin="9"/>
<wire x1="22.86" y1="-2.54" x2="15.24" y2="-2.54" width="0.1524" layer="91"/>
<wire x1="15.24" y1="-2.54" x2="15.24" y2="-17.78" width="0.1524" layer="91"/>
<pinref part="SV3" gate="G$1" pin="10"/>
<wire x1="15.24" y1="-17.78" x2="22.86" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="SV3" gate="G$1" pin="7"/>
<wire x1="25.4" y1="-2.54" x2="25.4" y2="0" width="0.1524" layer="91"/>
<wire x1="25.4" y1="0" x2="12.7" y2="0" width="0.1524" layer="91"/>
<wire x1="12.7" y1="0" x2="12.7" y2="-20.32" width="0.1524" layer="91"/>
<pinref part="SV3" gate="G$1" pin="8"/>
<wire x1="12.7" y1="-20.32" x2="25.4" y2="-20.32" width="0.1524" layer="91"/>
<wire x1="25.4" y1="-20.32" x2="25.4" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="SV3" gate="G$1" pin="5"/>
<wire x1="27.94" y1="-2.54" x2="27.94" y2="2.54" width="0.1524" layer="91"/>
<wire x1="27.94" y1="2.54" x2="10.16" y2="2.54" width="0.1524" layer="91"/>
<wire x1="10.16" y1="2.54" x2="10.16" y2="-22.86" width="0.1524" layer="91"/>
<wire x1="10.16" y1="-22.86" x2="27.94" y2="-22.86" width="0.1524" layer="91"/>
<pinref part="SV3" gate="G$1" pin="6"/>
<wire x1="27.94" y1="-22.86" x2="27.94" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="SV3" gate="G$1" pin="3"/>
<wire x1="30.48" y1="-2.54" x2="30.48" y2="5.08" width="0.1524" layer="91"/>
<wire x1="30.48" y1="5.08" x2="7.62" y2="5.08" width="0.1524" layer="91"/>
<wire x1="7.62" y1="5.08" x2="7.62" y2="-25.4" width="0.1524" layer="91"/>
<wire x1="7.62" y1="-25.4" x2="30.48" y2="-25.4" width="0.1524" layer="91"/>
<pinref part="SV3" gate="G$1" pin="4"/>
<wire x1="30.48" y1="-25.4" x2="30.48" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="6.3" minversion="6.2.2" severity="warning">
Since Version 6.2.2 text objects can contain more than one line,
which will not be processed correctly with this version.
</note>
</compatibility>
</eagle>
