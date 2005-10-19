#include        "avrx.inc"
/*
Copyright ©1998 - 2002 Larry Barello (larry@barello.net)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/copyleft/lgpl.html


REVISION HISTORY

20010403- Ported to avrx-ctoasm.inc

*/
        _MODULE(avrx_semaphores)

        _EXTERN(IntProlog)
        _EXTERN(AvrXKernelData)
        _EXTERN(_QueuePid)
        _EXTERN(_Epilog)

        _CODESECTION

/*+
; --------------------------------------------------
; void AvrXWaitObjectSemaphore(pMutex)
; void AvrXWaitTimer:
; void AvrXWaitMessageAck:
; void AvrXWaitTask:
;
; Standard system objects have a semaphore as their
; second byte.  This call waits on that.
;
; PASSED:       p1h:p1l = Object
; RETURNS:
; USES:         Z, X;
; CALLS:
; ASSUMES:
; NOTES:        - 6/28/00 Changed code to only do IntProlog/Epilog
;               if Semaphore is _PEND or > _DONE
;               - Must stay critical through entire routine so we
;               don't "append" ourselves to a "DONE" semaphore
;               - Because of above, can assume the running code is
;               at the top of the run queue (change code later..)
;
;               Trivial case ~20 cycles
-*/
        _FUNCTION(AvrXWaitObjectSemaphore)
        _PUBLIC(AvrXWaitTimer)
        _PUBLIC(AvrXWaitMessageAck)
        _PUBLIC(AvrXWaitTask)

AvrXWaitTimer:
AvrXWaitMessageAck:
AvrXWaitTask:
AvrXWaitObjectSemaphore:
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        _ENDFUNC
/*+
; -----------------------------------------------
; void AvrXWaitSemaphore(pMutex)
;
; Tasks will queue up for a Semaphore, thus implementing
; a Mutual Exclusion Semaphore.  For just plain old signaling
; only one task should wait on a semaphore.
;
; PASSED:       p1 =  Semaphore
; USES:         Z, tmp0, tmp1
; Returns:      void
; STACK:        One Context
;
; Notes:        Only called from user mode, may block.
-*/

        _FUNCTION(AvrXWaitSemaphore)

AvrXWaitSemaphore:
        mov     Zl, p1l
        mov     Zh, p1h

        BeginCritical           ; Must stay critical through entire routine

        ldd     tmp0, Z+NextL   ; Get semaphore
        ldd     tmp1, Z+NextH
        subi    tmp0, lo8(_DONE)
        sbci    tmp1, hi8(_DONE); Test trivial case of _DONE
        brne    aws01

        std     Z+NextL, tmp0   ; Reset Semaphore to _PEND
        std     Z+NextH, tmp1
        EndCriticalReturn       ; and return
aws01:
;        subi    tmp0, lo8(-_DONE)
;        sbci    tmp1, hi8(-_DONE); Restore semaphore

        rcall   IntProlog       ; Do task switch (ints disabled)

        ; With new code, we *can* assume we are at the top of the run queue

        ldi     Zl, lo8(AvrXKernelData+RunQueue)
        ldi     Zh, hi8(AvrXKernelData+RunQueue)
        ldd     p2h, Z+Running+NextH
        ldd     p2l, Z+Running+NextL
        rcall   _RemoveObject   ; Remove ourself from the run queue
        mov     Zl, p1l
        mov     Zh, p1h
        rcall   _AppendObject   ; Append ourselves to the Semaphore

        rjmp    _Epilog
        _ENDFUNC
/*+
; -----------------------------------------------
; void AvrXSetSemaphore(pMutex)
; void AvrXSetObjectSemaphore(pMythicalObject)
; void AvrXAckMessage(pMessageControlBlock)
;
; Sets (signals) an regular or object semaphore
;
; Signals a semaphore.  If semaphore is _PEND or _DONE, just mark
; DONE and return.  If a task is waiting, run it and set the semaphore
; to the _PEND state.
;
; Second Entry point is for Interrupt routines that want to Signal
; a semaphore.  Assumes context already saved.
;
; PASSED:       R25:R24 = Semaphore
; RETURNS:
; USES:         Everything
; CALLS:        AvrXIntSetSemaphore
; STACK:
; SIDE EFFECTS:
-*/
        _FUNCTION(AvrXSetObjectSemaphore)
        _PUBLIC(AvrXAckMessage)

AvrXAckMessage:
AvrXSetObjectSemaphore:
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        _ENDFUNC

        _FUNCTION(AvrXSetSemaphore)
AvrXSetSemaphore:
        rcall   AvrXIntSetSemaphore
        tst     r1l
        breq    axss0           ; Jump if top of run queue changed.
        ret
axss0:
        BeginCritical           ; Do task switch with interrupts off
        rcall   IntProlog
        EndCritical             ; Allow (any) pending interrupt to be serviced.
        rjmp    _Epilog

        _ENDFUNC
/*+
; -----------------------------------------------
; (void) AvrXIntSetSemaphore(pMutex)
; (void) AvrXIntSetObjectSemaphore(pMutex)
;
; Entry point for interrupt handlers (or code already switched into
; the system stack)
;
; PASSED:       p1h:p1l = Semaphore
; RETURNS:      r1l != 0, nothing queued
;                    = 0, task queued
; USES:         everything,
; STACK:        2
; SIDE EFFECTS: Assumes p1l == r1l
; NOTES:        Doesn't accumulate sets.  If semaphore already set, the
;               just ignore it (not a counting semaphore)
-*/
        _FUNCTION(AvrXIntSetObjectSemaphore)
AvrXIntSetObjectSemaphore:      ; This one here because so small
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        _ENDFUNC

        _FUNCTION(AvrXIntSetSemaphore)

AvrXIntSetSemaphore:            ; Entry point for interrupt exit routines.
        mov     Zl, p1l
        mov     Zh, p1h
        ldi     p1l, lo8(_DONE)
        ldi     p1h, hi8(_DONE)

        BeginCritical

        ldd     p2h, Z+NextH
        ldd     p2l, Z+NextL
        cp      p1l, p2l
        cpc     p1h, p2h        ; if 0x02 or higher...
        brlo    aiss00
        std     Z+NextL, p1l    ; Trivial case of _PEND or _DONE
        std     Z+NextH, p1h    ; Set to _DONE
BogusSemaphore:
        ldi     r1l, lo8(-1)	; Nothing queued
        EndCriticalReturn

aiss00:
        ldi     p1l, lo8(_LASTEV); Check for valid semaphore (e.g. >= $60)
;       ldi     p1h, hi8(_LASTEV); already zero...
        cp      p1l, p2l
        cpc     p1h, p2h
        brsh    BogusSemaphore

        rcall   _RemoveObjectAt ; Z->Prev, p2->Next (Object)

        EndCritical

        mov     p1l, p2l
        mov     p1h, p2h
        rjmp   _QueuePid       ; p1h:p1l = Pid, r1l = queued status

        _ENDFUNC

/*+-----------------------------------------------------------
        READ FUNCTION HEADERS AND CODE BEFORE USING!!!

        _AppendObject
        _RemoveObject
        _RemoveFirstObject
        _RemoveObjectAt

        Have the following call/return conventions:

        Z = pointer to head of queue/removal point
        p2l:p2l = Pointer to Object Passed/Returned
        Z = pointer to next object on return.

        These functions are called by higher level AvrX functions
        that have the object pointer in P2, hence the use of P2 so
        calls can be made without shuffling data.
-*/
/*+
; -------------------------------------------------
; _AppendObject
;
; Appends an object to a queue
;
; PASSED:       Z = Queue head
;               p2h:p2l = Object
; RETURNS:
; USES:         Z, & Flags
; CALLS:
; ASSUMES:      Null terminated list;
; NOTES:
-*/
_ao00:
        mov     Zl, tmp0
        mov     Zh, tmp1
        _FUNCTION(_AppendObject)
_AppendObject:
        ldd     tmp0, Z+NextL
        ldd     tmp1, Z+NextH
        subi    tmp0, lo8(0)
        sbci    tmp1, hi8(0)    ; Zip to end of list
        brne    _ao00

        std     Z+NextH, p2h
        std     Z+NextL, p2l   ; Prev->Next = Object
        mov     Zh, p2h
        mov     Zl, p2l
        std     Z+NextH, tmp1   ; Object->Next = 0
        std     Z+NextL, tmp0
        ret
        _ENDFUNC
/*+
; --------------------------------------------------
; _RemoveObject
;
; Removes an object from a queue.  Returns 0 if failed.
;
; PASSED:       Z         = Queue Head
;               p2h:p2l   = Object
; RETURNS:      p2h:p2l   = Object
;               tmp1:tmp0 = Object or 0
;               Z         = Next object in list
;               Z Flag set if Failed
;               Z Flag cleared if success
;
; USES:         Z, Flags
; CALLS:
; ASSUMES:      Called within a critical section
; NOTES:        Z are side effects used else where in AvrX
-*/
_ro00:
        mov     Zh, tmp1
        mov     Zl, tmp0
        _FUNCTION(_RemoveObject)
_RemoveObject:
        ldd     tmp0, Z+NextL
        ldd     tmp1, Z+NextH
        cp      p2l, tmp0
        cpc     p2h, tmp1
        breq    _RemoveObjectAt ; Match, we got it.
        subi    tmp0, lo8(0)
        sbci    tmp1, hi8(0)    ; Test end of list
        brne    _ro00           ; Walk the list
        ret                     ; End of list
        _ENDFUNC

/*+
; --------------------------------------------------
; pQueue _RemoveFirstObject(pQueue)
;
; Removes the first item in a queue. Return Zero if empty.
;
; PASSED:       Z       = Queue head
; RETURNS:      p2h:p2l = 0 if list empty, otherwize
;               Z       = Next Item
;               Zero Flag set if P2 == 0
;               Zero flag cleared if P2 != 0
;
; USES:         Flags
; CALLS:
; ASSUMES:      Null terminated list
; NOTES:        Interrupts NOT disabled.  Must disable if possibility
;               of something being insterted at head of queue
-*/
        _FUNCTION(_RemoveFirstObject)
_RemoveFirstObject:
        ldd     p2l, Z+NextL
        ldd     p2h, Z+NextH     ; X = pointer to object
        subi    p2l, lo8(0)
        sbci    p2h, hi8(0)      ; Test end of list
        brne    _RemoveObjectAt
        ret
        _ENDFUNC

/*+
; --------------------------------------------------
; pQueue _RemoveObjectAt(pQueue)
;
; Removes the  item in a queue.
;
; PASSED:       Z       = Prev
;               p2h:p2l = Object
;
; RETURNS:      p2h:p2l = Object
;               Z       = Next Item
;               Zero flag cleared
;
; USES:         Flags, tmp0-2
; CALLS:
; ASSUMES:      Null terminated list, valid object pointer
; NOTES:        Interrupts NOT disabled.  Must disable if possibility
;               of something being insterted at head of queue
-*/
        _FUNCTION(_RemoveObjectAt)

_RemoveObjectAt:                ; Z = Prev, p2 = Object
        mov     tmp2, Yl
        mov     tmp3, Yh
        mov     Yl, p2l
        mov     Yh, p2h

        ldd     p2h, Y+NextH
        ldd     p2l, Y+NextL
        std     Z+NextH, p2h
        std     Z+NextL, p2l    ; Prev->Next = Object->Next
        mov     Zl, p2l         ; Return Next in Z
        mov     Zh, p2h
        clr     p2l
        std     Y+NextL, p2l    ; Object->Next = 0
        std     Y+NextH, p2l

        mov     p2h, Yh         ; restore p2 & Y
        mov     p2l, Yl
        mov     Yl, tmp2
        mov     Yh, tmp3
        clz                     ; Return non-zero
        ret

        _ENDFUNC
        _END

