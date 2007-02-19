HOME=/home/hjh/home
DEST=/home/hjh/tmp/avrfreak

mkdir  $DEST
mkdir  $DEST/home
mkdir  $DEST/home/cc
mkdir  $DEST/home/cc/lq
mkdir  $DEST/home/cc/lq/tc
mkdir  $DEST/home/cc/include
mkdir  $DEST/home/cc/qq
mkdir  $DEST/home/cc/qq/avr
mkdir  $DEST/home/cc/qq/tc
mkdir  $DEST/home/cc/qq/avr/qqtshar
mkdir  $DEST/home/cc/qq/avr/qqtsfloa
mkdir  $DEST/home/cc/qq/mdos50
mkdir  $DEST/home/cc/qq/mdos50/qqtstrm
mkdir  $DEST/home/cc/avrbat
mkdir  $DEST/home/cc/tcbat
mkdir  $DEST/home/cc/djbat
mkdir  $DEST/home/cc/tc
mkdir  $DEST/home/cc/linux
mkdir  $DEST/home/html
mkdir  $DEST/home/html/hjh
mkdir  $DEST/home/html/hjh/cc
mkdir  $DEST/home/html/hjh/cc/english
mkdir  $DEST/home/html/hjh/cc/english/lq
mkdir  $DEST/home/html/hjh/cc/english/qq
mkdir  $DEST/home/html/hjh/cc/deutsch
mkdir  $DEST/home/html/hjh/cc/deutsch/lq
mkdir  $DEST/home/html/hjh/cc/deutsch/qq

cd $HOME/cc


cp -a linux/tl $DEST/home/cc/linux
cp -a linux/tll $DEST/home/cc/linux
cp -a linux/lbr $DEST/home/cc/linux
cp -a linux/lnk $DEST/home/cc/linux
cp -a linux/lnk2 $DEST/home/cc/linux
cp -a linux/lnk3 $DEST/home/cc/linux
cp -a linux/ccs $DEST/home/cc/linux
cp -a linux/ccsif $DEST/home/cc/linux

cp -a tcbat/as.bat  $DEST/home/cc/tcbat
cp -a tcbat/ccs.bat  $DEST/home/cc/tcbat
cp -a tcbat/lbr.bat  $DEST/home/cc/tcbat
cp -a tcbat/lnk.bat  $DEST/home/cc/tcbat


cp -a avr.bat $DEST/home/cc
cp -a lq/_addsf3.s $DEST/home/cc/lq
cp -a lq/_divsf3.s $DEST/home/cc/lq
cp -a lq/_eqsf2.s $DEST/home/cc/lq
cp -a lq/_floatsi.s $DEST/home/cc/lq
cp -a lq/_isnan.s $DEST/home/cc/lq
cp -a lq/_mulsf3.s $DEST/home/cc/lq
cp -a lq/asctofl.cpp $DEST/home/cc/lq
cp -a lq/asctohx2.cpp $DEST/home/cc/lq
cp -a lq/asctostr.cpp $DEST/home/cc/lq
cp -a lq/asctoul.cpp $DEST/home/cc/lq
cp -a lq/asctouw.cpp $DEST/home/cc/lq
cp -a lq/daycount.cpp $DEST/home/cc/lq
cp -a lq/dprbs.cpp $DEST/home/cc/lq
cp -a lq/epochepo.cpp $DEST/home/cc/lq
cp -a lq/etodate.cpp $DEST/home/cc/lq
cp -a lq/fillchar.cpp $DEST/home/cc/lq
cp -a lq/fmulpow2.s $DEST/home/cc/lq
cp -a lq/getdatet.cpp $DEST/home/cc/lq
cp -a lq/getticks.cpp $DEST/home/cc/lq
cp -a lq/getdatet.cpp $DEST/home/cc/lq
cp -a lq/int10.cpp $DEST/home/cc/lq
cp -a lq/int10.s $DEST/home/cc/lq
cp -a lq/itolower.cpp $DEST/home/cc/lq
cp -a lq/leapyear.cpp $DEST/home/cc/lq
cp -a lq/logprint.cpp $DEST/home/cc/lq
cp -a lq/logvt.cpp $DEST/home/cc/lq
cp -a lq/lprbs.cpp $DEST/home/cc/lq
cp -a lq/nsprintf.cpp $DEST/home/cc/lq
cp -a lq/serargs.cpp $DEST/home/cc/lq
cp -a lq/serial.cpp $DEST/home/cc/lq
cp -a lq/txtcpy.cpp $DEST/home/cc/lq
cp -a lq/uprintf.cpp $DEST/home/cc/lq
cp -a lq/vt.cpp $DEST/home/cc/lq
cp -a lq/vtcompil.cpp $DEST/home/cc/lq
cp -a lq/vtgetlin.cpp $DEST/home/cc/lq
cp -a lq/vthex2.cpp $DEST/home/cc/lq
cp -a lq/vtprintf.cpp $DEST/home/cc/lq
cp -a lq/todate.cpp $DEST/home/cc/lq
cp -a lq/tshar.cpp $DEST/home/cc/lq
cp -a lq/tsser.cpp $DEST/home/cc/lq
cp -a lq/tssfldec.cpp $DEST/home/cc/lq
cp -a lq/tssflout.cpp $DEST/home/cc/lq
cp -a lq/tssgetln.cpp $DEST/home/cc/lq
cp -a lq/tsshexo.cpp $DEST/home/cc/lq
cp -a lq/tssuchex.cpp $DEST/home/cc/lq
cp -a lq/tssulout.cpp $DEST/home/cc/lq
cp -a lq/tssuwout.cpp $DEST/home/cc/lq
cp -a lq/sin.cpp $DEST/home/cc/lq
cp -a lq/fmulpow2.s $DEST/home/cc/lq
cp -a lq/hmake.txt $DEST/home/cc/lq

cp -a include/cfifo.h $DEST/home/cc/include
cp -a include/hedeb.h $DEST/home/cc/include
cp -a include/helq.h $DEST/home/cc/include
cp -a include/hegettim.h $DEST/home/cc/include
cp -a include/helog.h $DEST/home/cc/include
cp -a include/heserial.h $DEST/home/cc/include
cp -a include/hegr.h $DEST/home/cc/include
cp -a include/heheap.h $DEST/home/cc/include
cp -a include/helang.h $DEST/home/cc/include
cp -a include/hemath.h $DEST/home/cc/include
cp -a include/heimath.h $DEST/home/cc/include
cp -a include/hevt.h $DEST/home/cc/include
cp -a include/heinitf.h $DEST/home/cc/include
cp -a include/heport.h $DEST/home/cc/include
cp -a include/heserial.h $DEST/home/cc/include
cp -a include/st.h $DEST/home/cc/include
cp -a include/tshar.h $DEST/home/cc/include
cp -a include/tsser.h $DEST/home/cc/include
cp -a include/longbr.inc $DEST/home/cc/include


cp -a favrfrek.sh $DEST/home/cc
cp -a tc.bat $DEST/home/cc
cp -a lccwin32.bat $DEST/home/cc
cp -a dj.bat $DEST/home/cc


cp -a qq/qqtsfloa.cpp $DEST/home/cc/qq
cp -a qq/qqtstrm.cpp $DEST/home/cc/qq
cp -a qq/mdos50/qqtstrm/qqtstrm.dsw $DEST/home/cc/qq/mdos50/qqtstrm
cp -a qq/mdos50/qqtstrm/qqtstrm.dsp $DEST/home/cc/qq/mdos50/qqtstrm
cp -a qq/tc/qqtstrm.dsk $DEST/home/cc/qq/tc
cp -a qq/tc/qqtstrm.prj $DEST/home/cc/qq/tc
cp -a qq/avr/qqtsfloa/makefile $DEST/home/cc/qq/avr/qqtsfloa
cp -a qq/avr/qqtsfloa/qqtsfloa.pnproj $DEST/home/cc/qq/avr/qqtsfloa
cp -a qq/avr/qqtsfloa/qqtsfloa.pnproj $DEST/home/cc/qq/avr/qqtsfloa


cp -a avrbat/* $DEST/home/cc/avrbat

cd $HOME/html/hjh/cc/deutsch
cp -a avrlin.htm $DEST/home/html/hjh/cc/deutsch
cp -a avrstud.htm $DEST/home/html/hjh/cc/deutsch
cp -a winavr.htm $DEST/home/html/hjh/cc/deutsch

cd $HOME/html/hjh/cc/deutsch/lq
cp -a serial.htm $DEST/home/html/hjh/cc/deutsch/lq
cp -a _addsf3.htm $DEST/home/html/hjh/cc/deutsch/lq

cd $HOME/html/hjh/cc/deutsch/qq
cp -a qqtsser.htm $DEST/home/html/hjh/cc/deutsch/qq
cp -a qqtstrm.htm $DEST/home/html/hjh/cc/deutsch/qq


cd $HOME/html/hjh/cc/english
cp -a avrstud.htm $DEST/home/html/hjh/cc/english
cp -a compile.htm $DEST/home/html/hjh/cc/english
cp -a linux.htm $DEST/home/html/hjh/cc/english
cp -a msvc10.htm $DEST/home/html/hjh/cc/english
cp -a msvc40.htm $DEST/home/html/hjh/cc/english
cp -a msvc50.htm $DEST/home/html/hjh/cc/english
cp -a lccwin32.htm $DEST/home/html/hjh/cc/english
cp -a oskgcc.htm $DEST/home/html/hjh/cc/english
cp -a tcinst.htm $DEST/home/html/hjh/cc/english
cp -a turbocpp.htm $DEST/home/html/hjh/cc/english
cp -a winavr.htm $DEST/home/html/hjh/cc/english
cp -a www.htm $DEST/home/html/hjh/cc/english


cd $HOME/html/hjh/cc/english/lq
cp -a _addsf3.htm $DEST/home/html/hjh/cc/english/lq


cd $HOME/html/hjh/cc/english/qq
cp -a qqtstrm.htm qqtsfloa.htm $DEST/home/html/hjh/cc/english/qq

