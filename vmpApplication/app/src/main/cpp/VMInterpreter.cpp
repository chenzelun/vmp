//
// Created by ChenDalunlun on 2019/8/27.
//

#include "VMInterpreter.h"

#include <math.h>
#include "Util.h"
#include "Dex.h"
#include <algorithm>

extern JNIEnv *env;


void
dvmInterpret(JNIEnv *env, jobject instance, const VmMethod *curMethod, jvalue *reg,
             jvalue *pResult) {

    /* core state */
    const u2 *pc;               // program counter
    const u2 *curInsns = curMethod->code->insns;
    u2 inst;                    // current instruction
    /* instruction decoding */
    u4 ref;               // 16 or 32-bit quantity fetched directly
    u2 vsrc1, vsrc2, vdst;      // usually used for register indexes
    jvalue retval;
    retval.j = 0;

    jthrowable curException = nullptr;

    bool methodCallRange = false;
    jmethodID methodToCall = nullptr;
    VmMethod *vmMethodToCall = new VmMethod();
    MethodType methodToCallType = METHOD_UNKNOWN;
    jclass methodToCallClazz = nullptr;

    pc = curInsns;

    u4 curOp;

    FINISH(0);                  /* fetch and execute first instruction */

    NEXT_INSNS:
    switch (curOp) {
        // 0x00-0x0c
        case OP_NOP: HANDLE_OPCODE(OP_NOP)
            FINISH(1);
        OP_END

        case OP_MOVE: HANDLE_OPCODE(OP_MOVE /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOG_D("|move%s v%d,v%d %s(v%d=0x%08x)",
                  (INST_INST(inst) == OP_MOVE) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(1);
        OP_END

        case OP_MOVE_FROM16: HANDLE_OPCODE(OP_MOVE_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|move%s/from16 v%d,v%d %s(v%d=0x%08x)",
                  (INST_INST(inst) == OP_MOVE_FROM16) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(2);
        OP_END

        case OP_MOVE_16: HANDLE_OPCODE(OP_MOVE_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOG_D("|move%s/16 v%d,v%d %s(v%d=0x%08x)",
                  (INST_INST(inst) == OP_MOVE_16) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(3);
        OP_END

        case OP_MOVE_WIDE: HANDLE_OPCODE(OP_MOVE_WIDE /*vA, vB*/)
            /* IMPORTANT: must correctly handle overlapping registers, e.g. both
            * "move-wide v6, v7" and "move-wide v7, v6" */
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOG_D("|move-wide v%d,v%d %s(v%d=0x%016lx)", vdst, vsrc1,
                  kSpacing + 5, vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(1);
        OP_END

        case OP_MOVE_WIDE_FROM16: HANDLE_OPCODE(OP_MOVE_WIDE_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|move-wide/from16 v%d,v%d  (v%d=0x%016lx)", vdst, vsrc1,
                  vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(2);
        OP_END

        case OP_MOVE_WIDE_16: HANDLE_OPCODE(OP_MOVE_WIDE_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOG_D("|move-wide/16 v%d,v%d %s(v%d=0x%016lx)", vdst, vsrc1,
                  kSpacing + 8, vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(3);
        OP_END

        case OP_MOVE_OBJECT: HANDLE_OPCODE(OP_MOVE_OBJECT /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOG_D("|move%s v%d,v%d %s(v%d=%p)",
                  (INST_INST(inst) == OP_MOVE) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            SET_REGISTER_AS_OBJECT(vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            FINISH(1);
        OP_END

        case OP_MOVE_OBJECT_FROM16: HANDLE_OPCODE(OP_MOVE_OBJECT_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|move%s/from16 v%d,v%d %s(v%d=%p)",
                  (INST_INST(inst) == OP_MOVE_FROM16) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            SET_REGISTER_AS_OBJECT(vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            FINISH(2);
        OP_END

        case OP_MOVE_OBJECT_16: HANDLE_OPCODE(OP_MOVE_OBJECT_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOG_D("|move%s/16 v%d,v%d %s(v%d=%p)",
                  (INST_INST(inst) == OP_MOVE_16) ? "" : "-object", vdst, vsrc1,
                  kSpacing, vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            SET_REGISTER_AS_OBJECT(vdst, GET_REGISTER_AS_OBJECT(vsrc1));
            FINISH(3);
        OP_END

        case OP_MOVE_RESULT: HANDLE_OPCODE(OP_MOVE_RESULT /*vAA*/)
            vdst = INST_AA(inst);
            LOG_D("|move-result%s v%d %s(v%d=0x%08x)",
                  (INST_INST(inst) == OP_MOVE_RESULT) ? "" : "-object",
                  vdst, kSpacing + 4, vdst, retval.i);
            SET_REGISTER(vdst, retval.i);
            FINISH(1);
        OP_END

        case OP_MOVE_RESULT_WIDE: HANDLE_OPCODE(OP_MOVE_RESULT_WIDE /*vAA*/)
            vdst = INST_AA(inst);
            LOG_D("|move-result-wide v%d %s(0x%016lx)", vdst, kSpacing, retval.j);
            SET_REGISTER_WIDE(vdst, retval.j);
            FINISH(1);
        OP_END

        case OP_MOVE_RESULT_OBJECT: HANDLE_OPCODE(OP_MOVE_RESULT_OBJECT /*vAA*/)
            vdst = INST_AA(inst);
            LOG_D("|move-result%s v%d %s(v%d=0x%p)",
                  (INST_INST(inst) == OP_MOVE_RESULT) ? "" : "-object",
                  vdst, kSpacing + 4, vdst, retval.l);
            SET_REGISTER_AS_OBJECT(vdst, retval.l);
            FINISH(1);
        OP_END

        case OP_MOVE_EXCEPTION: HANDLE_OPCODE(OP_MOVE_EXCEPTION /*vAA*/)
            vdst = INST_AA(inst);
            LOG_D("|move-exception v%d", vdst);
            assert(curException != NULL);
            SET_REGISTER_AS_OBJECT(vdst, curException);
            curException = nullptr;
            FINISH(1);
        OP_END

            // 0x0e-1c
        case OP_RETURN_VOID: HANDLE_OPCODE(OP_RETURN_VOID /**/)
            LOG_D("|return-void");
        OP_END
            break;

        case OP_RETURN: HANDLE_OPCODE(OP_RETURN /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOG_D("|return%s v%d", (INST_INST(inst) == OP_RETURN) ? "" : "-object", vsrc1);
            retval.j = 0;       //init
            retval.i = GET_REGISTER_INT(vsrc1);
        OP_END
            break;

        case OP_RETURN_WIDE: HANDLE_OPCODE(OP_RETURN_WIDE /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOG_D("|return-wide v%d", vsrc1);
            retval.j = GET_REGISTER_WIDE(vsrc1);
        OP_END
            break;

        case OP_RETURN_OBJECT: HANDLE_OPCODE(OP_RETURN_OBJECT /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOG_D("|return%s v%d",
                  (INST_INST(inst) == OP_RETURN) ? "" : "-object", vsrc1);
            retval.l = GET_REGISTER_AS_OBJECT(vsrc1);
        OP_END
            break;

        case OP_CONST_4: HANDLE_OPCODE(OP_CONST_4 /*vA, #+B*/)
            vdst = static_cast<u2>(INST_A(inst));
            s4 tmp = (s4) (INST_B(inst) << 28) >> 28;  // sign extend 4-bit value
            LOG_D("|const/4 v%d,#0x%02x", vdst, tmp);
            SET_REGISTER(vdst, tmp);
            FINISH(1);
        OP_END

        case OP_CONST_16: HANDLE_OPCODE(OP_CONST_16 /*vAA, #+BBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|const/16 v%d,#0x%04x", vdst, (s2) vsrc1);
            SET_REGISTER(vdst, (s2) vsrc1);
            FINISH(2);
        OP_END

        case OP_CONST: HANDLE_OPCODE(OP_CONST /*vAA, #+BBBBBBBB*/)
            vdst = INST_AA(inst);
            u4 tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOG_D("|const v%d,#0x%08x", vdst, tmp);
            SET_REGISTER(vdst, tmp);
            FINISH(3);
        OP_END

        case OP_CONST_HIGH16: HANDLE_OPCODE(OP_CONST_HIGH16 /*vAA, #+BBBB0000*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|const/high16 v%d,#0x%04x0000", vdst, vsrc1);
            SET_REGISTER(vdst, vsrc1 << 16);
            FINISH(2);
        OP_END

        case OP_CONST_WIDE_16: HANDLE_OPCODE(OP_CONST_WIDE_16 /*vAA, #+BBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|const-wide/16 v%d,#0x%04x", vdst, (s2) vsrc1);
            SET_REGISTER_WIDE(vdst, (s2) vsrc1);
            FINISH(2);
        OP_END

        case OP_CONST_WIDE_32: HANDLE_OPCODE(OP_CONST_WIDE_32 /*vAA, #+BBBBBBBB*/)
            u4 tmp;
            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOG_D("|const-wide/32 v%d,#0x%08x", vdst, tmp);
            SET_REGISTER_WIDE(vdst, (s4) tmp);
            FINISH(3);
        OP_END

        case OP_CONST_WIDE: HANDLE_OPCODE(OP_CONST_WIDE /*vAA, #+BBBBBBBB BBBBBBBB*/)
            u8 tmp;
            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u8) FETCH(2) << 16;
            tmp |= (u8) FETCH(3) << 32;
            tmp |= (u8) FETCH(4) << 48;
            LOG_D("|const-wide v%d,#0x%016lx", vdst, tmp);
            SET_REGISTER_WIDE(vdst, tmp);
            FINISH(5);
        OP_END

        case OP_CONST_WIDE_HIGH16: HANDLE_OPCODE(OP_CONST_WIDE_HIGH16 /*vAA, #+BBBB000000000000*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOG_D("|const-wide/high16 v%d,#0x%04x000000000000", vdst, vsrc1);
            SET_REGISTER_WIDE(vdst, ((u8) vsrc1) << 48);
            FINISH(2);
        OP_END

        case OP_CONST_STRING: HANDLE_OPCODE(OP_CONST_STRING /*vAA, string@BBBB*/)
            jstring strObj = nullptr;
            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOG_D("|const-string v%d string@0x%04x", vdst, ref);
            // get string from dex by stringId
            strObj = dvmResolveString(curMethod, ref);
            assert(strObj != nullptr);
            SET_REGISTER_AS_OBJECT(vdst, strObj);
            FINISH(2);
        OP_END

        case OP_CONST_STRING_JUMBO: HANDLE_OPCODE(OP_CONST_STRING_JUMBO /*vAA, string@BBBBBBBB*/)
            jstring strObj = nullptr;
            u4 tmp;
            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOG_D("|const-string/jumbo v%d string@0x%08x", vdst, tmp);
            // get string from dex by stringId
            strObj = dvmResolveString(curMethod, tmp);
            assert(strObj != nullptr);
            SET_REGISTER_AS_OBJECT(vdst, strObj);
            FINISH(3);
        OP_END

        case OP_CONST_CLASS: HANDLE_OPCODE(OP_CONST_CLASS /*vAA, class@BBBB*/)
            jclass clazz = nullptr;
            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOG_D("|const-class v%d class@0x%04x", vdst, ref);
            // get clazz from dex by clazzId
            clazz = dvmResolveClass(curMethod, ref);
            if (clazz == nullptr) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER_AS_OBJECT(vdst, clazz);
            FINISH(2);
        OP_END

            // 0x1d-1e
        case OP_MONITOR_ENTER: HANDLE_OPCODE(OP_MONITOR_ENTER /*vAA*/)
            jobject obj;
            vsrc1 = INST_AA(inst);
            LOG_D("|monitor-enter v%d %s(%p)",
                  vsrc1, kSpacing + 6, GET_REGISTER_AS_OBJECT(vsrc1));
            obj = GET_REGISTER_AS_OBJECT(vsrc1);
            if (!checkForNull(obj)) {
                GOTO_exceptionThrown();
            }
            (*env).MonitorEnter(obj);
            FINISH(1);
        OP_END

        case OP_MONITOR_EXIT: HANDLE_OPCODE(OP_MONITOR_EXIT /*vAA*/)
            jobject obj;
            vsrc1 = INST_AA(inst);
            LOG_D("|monitor-exit v%d %s(%p)",
                  vsrc1, kSpacing + 5, GET_REGISTER_AS_OBJECT(vsrc1));
            obj = GET_REGISTER_AS_OBJECT(vsrc1);
            if (!checkForNull(obj)) {
                /*
                 * The exception needs to be processed at the *following*
                 * instruction, not the current instruction (see the Dalvik
                 * spec).  Because we're jumping to an exception handler,
                 * we're not actually at risk of skipping an instruction
                 * by doing so.
                 */
                ADJUST_PC(1);           /* monitor-exit width is 1 */
                GOTO_exceptionThrown();
            }
            if (!(*env).MonitorExit(obj)) {
                ADJUST_PC(1);
                GOTO_exceptionThrown();
            }
            FINISH(1);
        OP_END

            // 0x1f-26
        case OP_CHECK_CAST: HANDLE_OPCODE(OP_CHECK_CAST /*vAA, class@BBBB*/)
            jclass clazz = nullptr;
            jobject obj;
            vsrc1 = INST_AA(inst);
            ref = FETCH(1);         /* class to check against */
            LOG_D("|check-cast v%d,class@0x%04x", vsrc1, ref);
            obj = GET_REGISTER_AS_OBJECT(vsrc1);
            if (obj != nullptr) {
                //  get clazz from dex by clazzId
                clazz = dvmResolveClass(curMethod, ref);
                if (clazz == nullptr) {
                    GOTO_exceptionThrown();
                }

                if (!(*env).IsInstanceOf(obj, clazz)) {
                    dvmThrowClassCastException((*env).GetObjectClass(obj), clazz);
                    GOTO_exceptionThrown();
                }
            }
            FINISH(2);
        OP_END

        case OP_INSTANCE_OF: HANDLE_OPCODE(OP_INSTANCE_OF /*vA, vB, class@CCCC*/)
            jclass clazz = nullptr;
            jobject obj;

            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);   /* object to check */
            ref = FETCH(1);         /* class to check against */
            LOG_D("|instance-of v%d,v%d,class@0x%04x", vdst, vsrc1, ref);
            obj = GET_REGISTER_AS_OBJECT(vsrc1);
            if (obj == nullptr) {
                SET_REGISTER(vdst, 0);
            } else {
                // get clazz from dex by clazzId
                clazz = dvmResolveClass(curMethod, ref);
                if (clazz == nullptr) {
                    GOTO_exceptionThrown();
                }
            }
            // check type
            SET_REGISTER(vdst, static_cast<u4>((*env).IsInstanceOf(obj, clazz)));
            FINISH(2);
        OP_END

        case OP_ARRAY_LENGTH: HANDLE_OPCODE(OP_ARRAY_LENGTH /*vA, vB*/)
            jobjectArray arrayObj;
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            arrayObj = (jobjectArray) GET_REGISTER_AS_OBJECT(vsrc1);
            LOG_D("|array-length v%d,v%d  (%p)", vdst, vsrc1, arrayObj);
            if (!checkForNull(arrayObj)) {
                GOTO_exceptionThrown();
            }
            /* verifier guarantees this is an array reference */
            u4 arrayLen = static_cast<u4>((*env).GetArrayLength(arrayObj));
            SET_REGISTER(vdst, arrayLen);
            FINISH(1);
        OP_END

        case OP_NEW_INSTANCE: HANDLE_OPCODE(OP_NEW_INSTANCE /*vAA, class@BBBB*/)
            jclass clazz = nullptr;
            jobject newObj = nullptr;

            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOG_D("|new-instance v%d,class@0x%04x", vdst, ref);
            //  get clazz from dex by clazzId
            clazz = dvmResolveClass(curMethod, ref);
            if (clazz == nullptr) {
                GOTO_exceptionThrown();
            }
            newObj = (*env).AllocObject(clazz);
            if (!checkForNull(newObj)) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER_AS_OBJECT(vdst, newObj);
            FINISH(2);
        OP_END

        case OP_NEW_ARRAY: HANDLE_OPCODE(OP_NEW_ARRAY /*vA, vB, class@CCCC*/)
            jarray newArray;
            s4 length;

            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);       /* length reg */
            ref = FETCH(1);
            LOG_D("|new-array v%d,v%d,class@0x%04x  (%d elements)",
                  vdst, vsrc1, ref, GET_REGISTER(vsrc1));
            length = GET_REGISTER(vsrc1);
            if (length < 0) {
                dvmThrowNegativeArraySizeException(length);
                GOTO_exceptionThrown()
            }
            newArray = dvmAllocArrayByClass(length, curMethod, ref);
            if (newArray == nullptr) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER_AS_OBJECT(vdst, newArray);
            FINISH(2);
        OP_END

        case OP_FILLED_NEW_ARRAY: HANDLE_OPCODE(
                OP_FILLED_NEW_ARRAY /*vB, {vD, vE, vF, vG, vA}, class@CCCC*/)
            GOTO_invoke(filledNewArray, false);
        OP_END

        case OP_FILLED_NEW_ARRAY_RANGE: HANDLE_OPCODE(
                OP_FILLED_NEW_ARRAY_RANGE /*{vCCCC..v(CCCC+AA-1)}, class@BBBB*/)
            GOTO_invoke(filledNewArray, true);
        OP_END

        case OP_FILL_ARRAY_DATA: HANDLE_OPCODE(OP_FILL_ARRAY_DATA)   /*vAA, +BBBBBBBB*/
            const u2 *arrayData;
            s4 offset;
            jarray arrayObj;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOG_D("|fill-array-data v%d +0x%04x", vsrc1, offset);
            arrayData = pc + offset;       // offset in 16-bit units

            arrayObj = (jarray) GET_REGISTER_AS_OBJECT(vsrc1);
            if (!dvmInterpretHandleFillArrayData(arrayObj, arrayData)) {
                GOTO_exceptionThrown();
            }
            FINISH(3);
        OP_END


        case OP_THROW: HANDLE_OPCODE(OP_THROW /*vAA*/)
            jthrowable obj;

            vsrc1 = INST_AA(inst);
            LOG_D("|throw v%d  (%p)", vsrc1, GET_REGISTER_AS_OBJECT(vsrc1));
            obj = (jthrowable) GET_REGISTER_AS_OBJECT(vsrc1);
            if (!checkForNull(obj)) {
                /* will throw a null pointer exception */
                LOG_E("Bad exception");
                GOTO_exceptionThrown();
            } else {
                /* use the requested exception */
                (*env).Throw(obj);
                GOTO_exceptionThrown();
            }
        OP_END

            // 0x28-0x3d
        case OP_GOTO: HANDLE_OPCODE(OP_GOTO /*+AA*/)
            vdst = INST_AA(inst);
            if ((s1) vdst < 0) {
                LOG_D("|goto -0x%02x", -((s1) vdst));
            } else {
                LOG_D("|goto +0x%02x", ((s1) vdst));
                LOG_D("> branch taken");
            }
            if ((s1) vdst < 0) {PERIODIC_CHECKS((s1) vdst); }
            FINISH((s1) vdst);
        OP_END

        case OP_GOTO_16: HANDLE_OPCODE(OP_GOTO_16 /*+AAAA*/)
            s4 offset = (s2) FETCH(1);          /* sign-extend next code unit */
            if (offset < 0) {
                LOG_D("|goto/16 -0x%04x", -offset);
            } else {
                LOG_D("|goto/16 +0x%04x", offset);
            }
            LOG_D("> branch taken");
            if (offset < 0) {PERIODIC_CHECKS(offset); };
            FINISH(offset);
        OP_END

        case OP_GOTO_32: HANDLE_OPCODE(OP_GOTO_32 /*+AAAAAAAA*/)
            s4 offset = FETCH(1);               /* low-order 16 bits */
            offset |= ((s4) FETCH(2)) << 16;    /* high-order 16 bits */

            if (offset < 0) { LOG_D("|goto/32 -0x%08x", -offset); }
            else { LOG_D("|goto/32 +0x%08x", offset); }
            LOG_D("> branch taken");

            /* allowed to branch to self */
            if (offset <= 0) {PERIODIC_CHECKS(offset); }
            FINISH(offset);
        OP_END

        case OP_PACKED_SWITCH: HANDLE_OPCODE(OP_PACKED_SWITCH /*vAA, +BBBB*/)
            const u2 *switchData;
            u4 testVal;
            s4 offset;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOG_D("|packed-switch v%d +0x%04x", vsrc1, offset);
            switchData = pc + offset;       // offset in 16-bit units
            testVal = (u4) GET_REGISTER(vsrc1);
            offset = dvmInterpretHandlePackedSwitch(switchData, testVal);
            LOG_D("> branch taken (0x%04x)", offset);

            if (offset <= 0) {PERIODIC_CHECKS(offset); }
            FINISH(offset);
        OP_END

        case OP_SPARSE_SWITCH: HANDLE_OPCODE(OP_SPARSE_SWITCH /*vAA, +BBBB*/)
            const u2 *switchData;
            u4 testVal;
            s4 offset;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOG_D("|sparse-switch v%d +0x%04x", vsrc1, offset);
            switchData = pc + offset;       // offset in 16-bit units
            testVal = (u4) GET_REGISTER(vsrc1);
            offset = dvmInterpretHandleSparseSwitch(switchData, testVal);
            LOG_D("> branch taken (0x%04x)", offset);
            /* uncommon */
            if (offset <= 0) {PERIODIC_CHECKS(offset); }
            FINISH(offset);
        OP_END

        case OP_CMPL_FLOAT: HANDLE_OP_CMPX(OP_CMPL_FLOAT, "l-float", jfloat, _FLOAT, -1)
        OP_END

        case OP_CMPG_FLOAT: HANDLE_OP_CMPX(OP_CMPG_FLOAT, "g-float", jfloat, _FLOAT, 1)
        OP_END

        case OP_CMPL_DOUBLE: HANDLE_OP_CMPX(OP_CMPL_DOUBLE, "l-double", jdouble, _DOUBLE, -1)
        OP_END

        case OP_CMPG_DOUBLE: HANDLE_OP_CMPX(OP_CMPG_DOUBLE, "g-double", jdouble, _DOUBLE, 1)
        OP_END

        case OP_CMP_LONG: HANDLE_OP_CMPX(OP_CMP_LONG, "-long", s8, _WIDE, 0)
        OP_END

        case OP_IF_EQ: HANDLE_OP_IF_XX(OP_IF_EQ, "eq", ==)
        OP_END

        case OP_IF_NE: HANDLE_OP_IF_XX(OP_IF_NE, "ne", !=)
        OP_END

        case OP_IF_LT: HANDLE_OP_IF_XX(OP_IF_LT, "lt", <)
        OP_END

        case OP_IF_GE: HANDLE_OP_IF_XX(OP_IF_GE, "ge", >=)
        OP_END

        case OP_IF_GT: HANDLE_OP_IF_XX(OP_IF_GT, "gt", >)
        OP_END

        case OP_IF_LE: HANDLE_OP_IF_XX(OP_IF_LE, "le", <=)
        OP_END

        case OP_IF_EQZ: HANDLE_OP_IF_XXZ(OP_IF_EQZ, "eqz", ==)
        OP_END

        case OP_IF_NEZ: HANDLE_OP_IF_XXZ(OP_IF_NEZ, "nez", !=)
        OP_END

        case OP_IF_LTZ: HANDLE_OP_IF_XXZ(OP_IF_LTZ, "ltz", <)
        OP_END

        case OP_IF_GEZ: HANDLE_OP_IF_XXZ(OP_IF_GEZ, "gez", >=)
        OP_END

        case OP_IF_GTZ: HANDLE_OP_IF_XXZ(OP_IF_GTZ, "gtz", >)
        OP_END

        case OP_IF_LEZ: HANDLE_OP_IF_XXZ(OP_IF_LEZ, "lez", <=)
        OP_END

            // 0x44-0x6d
        case OP_AGET: HANDLE_OP_AGET(OP_AGET, "-normal", Int, jint,)
        OP_END

        case OP_AGET_WIDE: HANDLE_OP_AGET(OP_AGET_WIDE, "-wide", Long, jlong, _WIDE)
        OP_END

        case OP_AGET_OBJECT: HANDLE_OPCODE(OP_AGET_OBJECT /*vAA, vBB, vCC*/)
            jobjectArray arrayObj;
            u2 arrayInfo;
            vdst = INST_AA(inst);
            arrayInfo = FETCH(1);
            vsrc1 = static_cast<u2>(arrayInfo & 0xff);    /* array ptr */
            vsrc2 = arrayInfo >> 8;      /* index */
            LOG_D("|aget%s v%d,v%d,v%d", "-object", vdst, vsrc1, vsrc2);
            arrayObj = (jobjectArray) GET_REGISTER_AS_OBJECT(vsrc1);
            if (!checkForNull(arrayObj)) GOTO_exceptionThrown();
            if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {
                dvmThrowArrayIndexOutOfBoundsException((*env).GetArrayLength(arrayObj),
                                                       GET_REGISTER(vsrc2));
                GOTO_exceptionThrown();
            }
            SET_REGISTER_AS_OBJECT(vdst,
                                   (*env).GetObjectArrayElement(arrayObj, GET_REGISTER(vsrc2)));
            LOG_D("+ AGET[%d]=%#x", GET_REGISTER(vsrc2), GET_REGISTER(vdst));
            FINISH(2);
        OP_END

        case OP_AGET_BOOLEAN: HANDLE_OP_AGET(OP_AGET_BOOLEAN, "-boolean", Boolean, jboolean,)
        OP_END

        case OP_AGET_BYTE: HANDLE_OP_AGET(OP_AGET_BYTE, "-byte", Byte, jbyte,)
        OP_END

        case OP_AGET_CHAR: HANDLE_OP_AGET(OP_AGET_CHAR, "-char", Char, jchar,)
        OP_END

        case OP_AGET_SHORT: HANDLE_OP_AGET(OP_AGET_SHORT, "-short", Short, jshort,)
        OP_END

        case OP_APUT: HANDLE_OP_APUT(OP_APUT, "-normal", Int, jint,)
        OP_END

        case OP_APUT_WIDE: HANDLE_OP_APUT(OP_APUT_WIDE, "-wide", Long, jlong, _WIDE)
        OP_END

        case OP_APUT_OBJECT: HANDLE_OPCODE(OP_APUT_OBJECT /*vAA, vBB, vCC*/)
            jobjectArray arrayObj;
            jobject obj;
            u2 arrayInfo;
            vdst = INST_AA(inst);       /* AA: source value */
            arrayInfo = FETCH(1);
            vsrc1 = static_cast<u2>(arrayInfo & 0xff);   /* BB: array ptr */
            vsrc2 = arrayInfo >> 8;     /* CC: index */
            LOG_D("|aput%s v%d,v%d,v%d", "-object", vdst, vsrc1, vsrc2);
            arrayObj = (jobjectArray) GET_REGISTER_AS_OBJECT(vsrc1);
            if (!checkForNull((jobject) arrayObj)) GOTO_exceptionThrown();
            if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {
                dvmThrowArrayIndexOutOfBoundsException((*env).GetArrayLength(arrayObj),
                                                       GET_REGISTER(vsrc2));
                GOTO_exceptionThrown();
            }
            obj = (jobject) GET_REGISTER_AS_OBJECT(vdst);
            if (!checkForNull(obj)) {
                GOTO_exceptionThrown();
            }
            if (!dvmCanPutArrayElement(obj, arrayObj)) {
                dvmThrowArrayStoreExceptionIncompatibleElement(obj, arrayObj);
                GOTO_exceptionThrown();
            }
            LOG_D("+ APUT[%d]=0x%08x", GET_REGISTER(vsrc2), GET_REGISTER(vdst));
            (*env).SetObjectArrayElement(arrayObj, GET_REGISTER(vsrc2), obj);
            FINISH(2);
        OP_END

        case OP_APUT_BOOLEAN: HANDLE_OP_APUT(OP_APUT_BOOLEAN, "-boolean", Boolean, jboolean,)
        OP_END

        case OP_APUT_BYTE: HANDLE_OP_APUT(OP_APUT_BYTE, "-byte", Byte, jbyte,)
        OP_END

        case OP_APUT_CHAR: HANDLE_OP_APUT(OP_APUT_CHAR, "-char", Char, jchar,)
        OP_END

        case OP_APUT_SHORT: HANDLE_OP_APUT(OP_APUT_SHORT, "-short", Short, jshort,)
        OP_END

        case OP_IGET: HANDLE_IGET_X(OP_IGET, "-normal", s4,)
        OP_END

        case OP_IGET_WIDE: HANDLE_IGET_X(OP_IGET_WIDE, "-wide", s8, _WIDE)
        OP_END

        case OP_IGET_OBJECT: HANDLE_IGET_X(OP_IGET_OBJECT, "-object", s8, _AS_OBJECT)
        OP_END

        case OP_IGET_BOOLEAN: HANDLE_IGET_X(OP_IGET_BOOLEAN, "-boolean", s4,)
        OP_END

        case OP_IGET_BYTE: HANDLE_IGET_X(OP_IGET_BYTE, "-byte", s4,)
        OP_END

        case OP_IGET_CHAR: HANDLE_IGET_X(OP_IGET_CHAR, "-char", s4,)
        OP_END

        case OP_IGET_SHORT: HANDLE_IGET_X(OP_IGET_SHORT, "-short", s4,)
        OP_END

        case OP_IPUT: HANDLE_IPUT_X(OP_IPUT, "-normal",)
        OP_END

        case OP_IPUT_WIDE: HANDLE_IPUT_X(OP_IPUT_WIDE, "-wide", _WIDE)
        OP_END

        case OP_IPUT_OBJECT: HANDLE_IPUT_X(OP_IPUT_OBJECT, "-object", _AS_OBJECT)
        OP_END

        case OP_IPUT_BOOLEAN: HANDLE_IPUT_X(OP_IPUT_BOOLEAN, "-boolean",)
        OP_END

        case OP_IPUT_BYTE: HANDLE_IPUT_X(OP_IPUT_BYTE, "-byte",)
        OP_END

        case OP_IPUT_CHAR: HANDLE_IPUT_X(OP_IPUT_CHAR, "-char",)
        OP_END

        case OP_IPUT_SHORT: HANDLE_IPUT_X(OP_IPUT_SHORT, "-short",)
        OP_END

        case OP_SGET: HANDLE_SGET_X(OP_SGET, "-normal", s4,)
        OP_END

        case OP_SGET_WIDE: HANDLE_SGET_X(OP_SGET_WIDE, "-wide", s8, _WIDE)
        OP_END

        case OP_SGET_OBJECT: HANDLE_SGET_X(OP_SGET_OBJECT, "-object", s8, _AS_OBJECT)
        OP_END

        case OP_SGET_BOOLEAN: HANDLE_SGET_X(OP_SGET_BOOLEAN, "-boolean", s4,)
        OP_END

        case OP_SGET_BYTE: HANDLE_SGET_X(OP_SGET_BYTE, "-byte", s4,)
        OP_END

        case OP_SGET_CHAR: HANDLE_SGET_X(OP_SGET_CHAR, "-char", s4,)
        OP_END

        case OP_SGET_SHORT: HANDLE_SGET_X(OP_SGET_SHORT, "-short", s4,)
        OP_END

        case OP_SPUT: HANDLE_SPUT_X(OP_SPUT, "-normal",)
        OP_END

        case OP_SPUT_WIDE: HANDLE_SPUT_X(OP_SPUT_WIDE, "-wide", _WIDE)
        OP_END

        case OP_SPUT_OBJECT: HANDLE_SPUT_X(OP_SPUT_OBJECT, "-object", _AS_OBJECT)
        OP_END

        case OP_SPUT_BOOLEAN: HANDLE_SPUT_X(OP_SPUT_BOOLEAN, "-boolean",)
        OP_END

        case OP_SPUT_BYTE: HANDLE_SPUT_X(OP_SPUT_BYTE, "-byte",)
        OP_END

        case OP_SPUT_CHAR: HANDLE_SPUT_X(OP_SPUT_CHAR, "-char",)
        OP_END

        case OP_SPUT_SHORT: HANDLE_SPUT_X(OP_SPUT_SHORT, "-short",)
        OP_END

            // 0x6e-0x78
        case OP_INVOKE_VIRTUAL: HANDLE_OPCODE(
                OP_INVOKE_VIRTUAL /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeVirtual, false);
        OP_END

        case OP_INVOKE_SUPER: HANDLE_OPCODE(OP_INVOKE_SUPER /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeSuper, false);
        OP_END

        case OP_INVOKE_DIRECT: HANDLE_OPCODE(
                OP_INVOKE_DIRECT /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeDirect, false);
        OP_END

        case OP_INVOKE_STATIC: HANDLE_OPCODE(
                OP_INVOKE_STATIC /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeStatic, false);
        OP_END

        case OP_INVOKE_INTERFACE: HANDLE_OPCODE(
                OP_INVOKE_INTERFACE /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeInterface, false);
        OP_END


        case OP_INVOKE_VIRTUAL_RANGE: HANDLE_OPCODE(
                OP_INVOKE_VIRTUAL_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeVirtual, true);
        OP_END

        case OP_INVOKE_SUPER_RANGE: HANDLE_OPCODE(
                OP_INVOKE_SUPER_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeSuper, true);
        OP_END

        case OP_INVOKE_DIRECT_RANGE: HANDLE_OPCODE(
                OP_INVOKE_DIRECT_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeDirect, true);
        OP_END

        case OP_INVOKE_STATIC_RANGE: HANDLE_OPCODE(
                OP_INVOKE_STATIC_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeStatic, true);
        OP_END

        case OP_INVOKE_INTERFACE_RANGE: HANDLE_OPCODE(
                OP_INVOKE_INTERFACE_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeInterface, true);
        OP_END

            // 0x7b-0x80
        case OP_NEG_INT: HANDLE_UNOP(OP_NEG_INT, "neg-int", -, ,)
        OP_END

        case OP_NOT_INT: HANDLE_UNOP(OP_NOT_INT, "not-int", , ^
                0xffffffff,)
        OP_END

        case OP_NEG_LONG: HANDLE_UNOP(OP_NEG_LONG, "neg-long", -, , _WIDE)
        OP_END

        case OP_NOT_LONG: HANDLE_UNOP(OP_NOT_LONG, "not-long", , ^
                0xffffffffffffffffULL, _WIDE)
        OP_END

        case OP_NEG_FLOAT: HANDLE_UNOP(OP_NEG_FLOAT, "neg-float", -, , _FLOAT)
        OP_END

        case OP_NEG_DOUBLE: HANDLE_UNOP(OP_NEG_DOUBLE, "neg-double", -, , _DOUBLE)
        OP_END

            // 0x84-0x8f
        case OP_INT_TO_LONG: HANDLE_NUMCONV(OP_INT_TO_LONG, "int-to-long", _INT, _WIDE)
        OP_END

        case OP_INT_TO_FLOAT: HANDLE_NUMCONV(OP_INT_TO_FLOAT, "int-to-float", _INT, _FLOAT)
        OP_END

        case OP_INT_TO_DOUBLE: HANDLE_NUMCONV(OP_INT_TO_DOUBLE, "int-to-double", _INT, _DOUBLE)
        OP_END

        case OP_LONG_TO_INT: HANDLE_NUMCONV(OP_LONG_TO_INT, "long-to-int", _WIDE, _INT)
        OP_END

        case OP_LONG_TO_FLOAT: HANDLE_NUMCONV(OP_LONG_TO_FLOAT, "long-to-float", _WIDE, _FLOAT)
        OP_END

        case OP_LONG_TO_DOUBLE: HANDLE_NUMCONV(OP_LONG_TO_DOUBLE, "long-to-double", _WIDE, _DOUBLE)
        OP_END

        case OP_FLOAT_TO_INT: HANDLE_FLOAT_TO_INT(OP_FLOAT_TO_INT, "float-to-int",
                                                  float, _FLOAT, s4, _INT)
        OP_END

        case OP_FLOAT_TO_LONG: HANDLE_FLOAT_TO_INT(OP_FLOAT_TO_LONG, "float-to-long",
                                                   float, _FLOAT, s8, _WIDE)
        OP_END

        case OP_FLOAT_TO_DOUBLE: HANDLE_NUMCONV(OP_FLOAT_TO_DOUBLE, "float-to-double", _FLOAT,
                                                _DOUBLE)
        OP_END

        case OP_DOUBLE_TO_INT: HANDLE_FLOAT_TO_INT(OP_DOUBLE_TO_INT, "double-to-int",
                                                   double, _DOUBLE, s4, _INT)
        OP_END

        case OP_DOUBLE_TO_LONG: HANDLE_FLOAT_TO_INT(OP_DOUBLE_TO_LONG, "double-to-long",
                                                    double, _DOUBLE, s8, _WIDE)
        OP_END

        case OP_DOUBLE_TO_FLOAT: HANDLE_NUMCONV(OP_DOUBLE_TO_FLOAT, "double-to-float",
                                                _DOUBLE, _FLOAT)
        OP_END

        case OP_INT_TO_BYTE: HANDLE_INT_TO_SMALL(OP_INT_TO_BYTE, "byte", s1)
        OP_END

        case OP_INT_TO_CHAR: HANDLE_INT_TO_SMALL(OP_INT_TO_CHAR, "char", u2)
        OP_END

        case OP_INT_TO_SHORT: HANDLE_INT_TO_SMALL(OP_INT_TO_SHORT, "short", s2)
        OP_END

            //  0x90-0x97
        case OP_ADD_INT: HANDLE_OP_X_INT(OP_ADD_INT, "add", +, 0)
        OP_END

        case OP_SUB_INT: HANDLE_OP_X_INT(OP_SUB_INT, "sub", -, 0)
        OP_END

        case OP_MUL_INT: HANDLE_OP_X_INT(OP_MUL_INT, "mul", *, 0)
        OP_END

        case OP_DIV_INT: HANDLE_OP_X_INT(OP_DIV_INT, "div", /, 1)
        OP_END

        case OP_REM_INT: HANDLE_OP_X_INT(OP_REM_INT, "rem", %, 2)
        OP_END

        case OP_AND_INT: HANDLE_OP_X_INT(OP_AND_INT, "and", &, 0)
        OP_END

        case OP_OR_INT: HANDLE_OP_X_INT(OP_OR_INT, "or", |, 0)
        OP_END

        case OP_XOR_INT: HANDLE_OP_X_INT(OP_XOR_INT, "xor", ^, 0)
        OP_END

            // 0x98-0x9a
        case OP_SHL_INT: HANDLE_OP_SHX_INT(OP_SHL_INT, "shl", (s4), <<)
        OP_END

        case OP_SHR_INT: HANDLE_OP_SHX_INT(OP_SHR_INT, "shr", (s4), >>)
        OP_END

        case OP_USHR_INT: HANDLE_OP_SHX_INT(OP_USHR_INT, "ushr", (u4), >>)
        OP_END

            // 0x9b-0xa2
        case OP_ADD_LONG: HANDLE_OP_X_LONG(OP_ADD_LONG, "add", +, 0)
        OP_END

        case OP_SUB_LONG: HANDLE_OP_X_LONG(OP_SUB_LONG, "sub", -, 0)
        OP_END

        case OP_MUL_LONG: HANDLE_OP_X_LONG(OP_MUL_LONG, "mul", *, 0)
        OP_END

        case OP_DIV_LONG: HANDLE_OP_X_LONG(OP_DIV_LONG, "div", /, 1)
        OP_END

        case OP_REM_LONG: HANDLE_OP_X_LONG(OP_REM_LONG, "rem", %, 2)
        OP_END

        case OP_AND_LONG: HANDLE_OP_X_LONG(OP_AND_LONG, "and", &, 0)
        OP_END

        case OP_OR_LONG: HANDLE_OP_X_LONG(OP_OR_LONG, "or", |, 0)
        OP_END

        case OP_XOR_LONG: HANDLE_OP_X_LONG(OP_XOR_LONG, "xor", ^, 0)
        OP_END

            // 0xa3-0xa5
        case OP_SHL_LONG: HANDLE_OP_SHX_LONG(OP_SHL_LONG, "shl", (s8), <<)
        OP_END

        case OP_SHR_LONG: HANDLE_OP_SHX_LONG(OP_SHR_LONG, "shr", (s8), >>)
        OP_END

        case OP_USHR_LONG: HANDLE_OP_SHX_LONG(OP_USHR_LONG, "ushr", (u8), >>)
        OP_END

            // 0xa6-0xaa
        case OP_ADD_FLOAT: HANDLE_OP_X_FLOAT(OP_ADD_FLOAT, "add", +)
        OP_END

        case OP_SUB_FLOAT: HANDLE_OP_X_FLOAT(OP_SUB_FLOAT, "sub", -)
        OP_END

        case OP_MUL_FLOAT: HANDLE_OP_X_FLOAT(OP_MUL_FLOAT, "mul", *)
        OP_END

        case OP_DIV_FLOAT: HANDLE_OP_X_FLOAT(OP_DIV_FLOAT, "div", /)
        OP_END

        case OP_REM_FLOAT: HANDLE_OPCODE(OP_REM_FLOAT /*vAA, vBB, vCC*/)
            u2 srcRegs;
            vdst = INST_AA(inst);
            srcRegs = FETCH(1);
            vsrc1 = static_cast<u2>(srcRegs & 0xff);
            vsrc2 = srcRegs >> 8;
            LOG_D("|%s-float v%d,v%d,v%d", "mod", vdst, vsrc1, vsrc2);
            SET_REGISTER_FLOAT(vdst, fmodf(GET_REGISTER_FLOAT(vsrc1), GET_REGISTER_FLOAT(vsrc2)));
            FINISH(2);
        OP_END

            //0xab-0xaf
        case OP_ADD_DOUBLE: HANDLE_OP_X_DOUBLE(OP_ADD_DOUBLE, "add", +)
        OP_END

        case OP_SUB_DOUBLE: HANDLE_OP_X_DOUBLE(OP_SUB_DOUBLE, "sub", -)
        OP_END

        case OP_MUL_DOUBLE: HANDLE_OP_X_DOUBLE(OP_MUL_DOUBLE, "mul", *)
        OP_END

        case OP_DIV_DOUBLE: HANDLE_OP_X_DOUBLE(OP_DIV_DOUBLE, "div", /)
        OP_END

        case OP_REM_DOUBLE: HANDLE_OPCODE(OP_REM_DOUBLE /*vAA, vBB, vCC*/)
            u2 srcRegs;
            vdst = INST_AA(inst);
            srcRegs = FETCH(1);
            vsrc1 = static_cast<u2>(srcRegs & 0xff);
            vsrc2 = srcRegs >> 8;
            LOG_D("|%s-double v%d,v%d,v%d", "mod", vdst, vsrc1, vsrc2);
            SET_REGISTER_DOUBLE(vdst, fmod(GET_REGISTER_DOUBLE(vsrc1), GET_REGISTER_DOUBLE(vsrc2)));
            FINISH(2);
        OP_END

            // 0xb0-0xba
        case OP_ADD_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_ADD_INT_2ADDR, "add", +, 0)
        OP_END

        case OP_SUB_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_SUB_INT_2ADDR, "sub", -, 0)
        OP_END

        case OP_MUL_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_MUL_INT_2ADDR, "mul", *, 0)
        OP_END

        case OP_DIV_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_DIV_INT_2ADDR, "div", /, 1)
        OP_END

        case OP_REM_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_REM_INT_2ADDR, "rem", %, 2)
        OP_END

        case OP_AND_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_AND_INT_2ADDR, "and", &, 0)
        OP_END

        case OP_OR_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_OR_INT_2ADDR, "or", |, 0)
        OP_END

        case OP_XOR_INT_2ADDR: HANDLE_OP_X_INT_2ADDR(OP_XOR_INT_2ADDR, "xor", ^, 0)
        OP_END

        case OP_SHL_INT_2ADDR: HANDLE_OP_SHX_INT_2ADDR(OP_SHL_INT_2ADDR, "shl", (s4), <<)
        OP_END

        case OP_SHR_INT_2ADDR: HANDLE_OP_SHX_INT_2ADDR(OP_SHR_INT_2ADDR, "shr", (s4), >>)
        OP_END

        case OP_USHR_INT_2ADDR: HANDLE_OP_SHX_INT_2ADDR(OP_USHR_INT_2ADDR, "ushr", (u4), >>)
        OP_END

            //0xbb-0xc5
        case OP_ADD_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_ADD_LONG_2ADDR, "add", +, 0)
        OP_END

        case OP_SUB_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_SUB_LONG_2ADDR, "sub", -, 0)
        OP_END

        case OP_MUL_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_MUL_LONG_2ADDR, "mul", *, 0)
        OP_END

        case OP_DIV_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_DIV_LONG_2ADDR, "div", /, 1)
        OP_END

        case OP_REM_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_REM_LONG_2ADDR, "rem", %, 2)
        OP_END

        case OP_AND_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_AND_LONG_2ADDR, "and", &, 0)
        OP_END

        case OP_OR_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_OR_LONG_2ADDR, "or", |, 0)
        OP_END

        case OP_XOR_LONG_2ADDR: HANDLE_OP_X_LONG_2ADDR(OP_XOR_LONG_2ADDR, "xor", ^, 0)
        OP_END

        case OP_SHL_LONG_2ADDR: HANDLE_OP_SHX_LONG_2ADDR(OP_SHL_LONG_2ADDR, "shl", (s8), <<)
        OP_END

        case OP_SHR_LONG_2ADDR: HANDLE_OP_SHX_LONG_2ADDR(OP_SHR_LONG_2ADDR, "shr", (s8), >>)
        OP_END

        case OP_USHR_LONG_2ADDR: HANDLE_OP_SHX_LONG_2ADDR(OP_USHR_LONG_2ADDR, "ushr", (u8), >>)
        OP_END

            // 0xc6-0xca
        case OP_ADD_FLOAT_2ADDR: HANDLE_OP_X_FLOAT_2ADDR(OP_ADD_FLOAT_2ADDR, "add", +)
        OP_END

        case OP_SUB_FLOAT_2ADDR: HANDLE_OP_X_FLOAT_2ADDR(OP_SUB_FLOAT_2ADDR, "sub", -)
        OP_END

        case OP_MUL_FLOAT_2ADDR: HANDLE_OP_X_FLOAT_2ADDR(OP_MUL_FLOAT_2ADDR, "mul", *)
        OP_END

        case OP_DIV_FLOAT_2ADDR: HANDLE_OP_X_FLOAT_2ADDR(OP_DIV_FLOAT_2ADDR, "div", /)
        OP_END

        case OP_REM_FLOAT_2ADDR: HANDLE_OPCODE(OP_REM_FLOAT_2ADDR /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOG_D("|%s-float-2addr v%d,v%d", "mod", vdst, vsrc1);
            SET_REGISTER_FLOAT(vdst, fmodf(GET_REGISTER_FLOAT(vdst), GET_REGISTER_FLOAT(vsrc1)));
            FINISH(1);
        OP_END

            // 0xcb-0xcf
        case OP_ADD_DOUBLE_2ADDR: HANDLE_OP_X_DOUBLE_2ADDR(OP_ADD_DOUBLE_2ADDR, "add", +)
        OP_END

        case OP_SUB_DOUBLE_2ADDR: HANDLE_OP_X_DOUBLE_2ADDR(OP_SUB_DOUBLE_2ADDR, "sub", -)
        OP_END

        case OP_MUL_DOUBLE_2ADDR: HANDLE_OP_X_DOUBLE_2ADDR(OP_MUL_DOUBLE_2ADDR, "mul", *)
        OP_END

        case OP_DIV_DOUBLE_2ADDR: HANDLE_OP_X_DOUBLE_2ADDR(OP_DIV_DOUBLE_2ADDR, "div", /)
        OP_END

        case OP_REM_DOUBLE_2ADDR: HANDLE_OPCODE(OP_REM_DOUBLE_2ADDR /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOG_D("|%s-double-2addr v%d,v%d", "mod", vdst, vsrc1);
            SET_REGISTER_DOUBLE(vdst, fmod(GET_REGISTER_DOUBLE(vdst), GET_REGISTER_DOUBLE(vsrc1)));
            FINISH(1);
        OP_END

            // 0xd0-0xd7
        case OP_ADD_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_ADD_INT_LIT16, "add", +, 0)
        OP_END

        case OP_RSUB_INT: HANDLE_OPCODE(OP_RSUB_INT /*vA, vB, #+CCCC*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            vsrc2 = FETCH(1);
            LOG_D("|rsub-int v%d,v%d,#+0x%04x", vdst, vsrc1, vsrc2);
            SET_REGISTER(vdst, (s2) vsrc2 - (s4) GET_REGISTER(vsrc1));
            FINISH(2);
        OP_END

        case OP_MUL_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_MUL_INT_LIT16, "mul", *, 0)
        OP_END

        case OP_DIV_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_DIV_INT_LIT16, "div", /, 1)
        OP_END

        case OP_REM_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_REM_INT_LIT16, "rem", %, 2)
        OP_END

        case OP_AND_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_AND_INT_LIT16, "and", &, 0)
        OP_END

        case OP_OR_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_OR_INT_LIT16, "or", |, 0)
        OP_END

        case OP_XOR_INT_LIT16: HANDLE_OP_X_INT_LIT16(OP_XOR_INT_LIT16, "xor", ^, 0)
        OP_END

            // 0xd8-0xe2
        case OP_ADD_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_ADD_INT_LIT8, "add", +, 0)
        OP_END

        case OP_RSUB_INT_LIT8: HANDLE_OPCODE(OP_RSUB_INT_LIT8 /*vAA, vBB, #+CC*/)
            u2 litInfo;
            vdst = INST_AA(inst);
            litInfo = FETCH(1);
            vsrc1 = static_cast<u2>(litInfo & 0xff);
            vsrc2 = litInfo >> 8;
            LOG_D("|%s-int/lit8 v%d,v%d,#+0x%02x", "rsub", vdst, vsrc1, vsrc2);
            SET_REGISTER(vdst, (s1) vsrc2 - (s4) GET_REGISTER(vsrc1));
            FINISH(2);
        OP_END

        case OP_MUL_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_MUL_INT_LIT8, "mul", *, 0)
        OP_END

        case OP_DIV_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_DIV_INT_LIT8, "div", /, 1)
        OP_END

        case OP_REM_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_REM_INT_LIT8, "rem", %, 2)
        OP_END

        case OP_AND_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_AND_INT_LIT8, "and", &, 0)
        OP_END

        case OP_OR_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_OR_INT_LIT8, "or", |, 0)
        OP_END

        case OP_XOR_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_XOR_INT_LIT8, "xor", ^, 0)
        OP_END

        case OP_SHL_INT_LIT8: HANDLE_OP_SHX_INT_LIT8(OP_SHL_INT_LIT8, "shl", (s4), <<)
        OP_END

        case OP_SHR_INT_LIT8: HANDLE_OP_SHX_INT_LIT8(OP_SHR_INT_LIT8, "shr", (s4), >>)
        OP_END

        case OP_USHR_INT_LIT8: HANDLE_OP_SHX_INT_LIT8(OP_USHR_INT_LIT8, "ushr", (u4), >>)
        OP_END

            // 0xe3-0xeb
        case OP_IGET_VOLATILE: HANDLE_IGET_X(OP_IGET_VOLATILE, "-volatile", s4,)
        OP_END

        case OP_IPUT_VOLATILE: HANDLE_IPUT_X(OP_IPUT_VOLATILE, "-volatile",)
        OP_END

        case OP_SGET_VOLATILE: HANDLE_SGET_X(OP_SGET_VOLATILE, "-volatile", s4,)
        OP_END

        case OP_SPUT_VOLATILE: HANDLE_SPUT_X(OP_SPUT_VOLATILE, "-volatile",)
        OP_END

        case OP_IGET_OBJECT_VOLATILE: HANDLE_IGET_X(OP_IGET_OBJECT_VOLATILE, "-object-volatile", s8,
                                                    _AS_OBJECT)
        OP_END

        case OP_IGET_WIDE_VOLATILE: HANDLE_IGET_X(OP_IGET_WIDE_VOLATILE, "-wide-volatile", s8,
                                                  _WIDE)
        OP_END

        case OP_IPUT_WIDE_VOLATILE: HANDLE_IPUT_X(OP_IPUT_WIDE_VOLATILE, "-wide-volatile", _WIDE)
        OP_END

        case OP_SGET_WIDE_VOLATILE: HANDLE_SGET_X(OP_SGET_WIDE_VOLATILE, "-wide-volatile", s8,
                                                  _WIDE)
        OP_END

        case OP_SPUT_WIDE_VOLATILE: HANDLE_SPUT_X(OP_SPUT_WIDE_VOLATILE, "-wide-volatile", _WIDE)
        OP_END

        case OP_THROW_VERIFICATION_ERROR: HANDLE_OPCODE(OP_THROW_VERIFICATION_ERROR)
            vsrc1 = INST_AA(inst);
            ref = FETCH(1);             /* class/field/method ref */
            dvmThrowVerificationError(curMethod, vsrc1, ref);
            GOTO_exceptionThrown();
        OP_END

        case OP_IPUT_OBJECT_VOLATILE: HANDLE_IPUT_X(OP_IPUT_OBJECT_VOLATILE, "-object-volatile",
                                                    _AS_OBJECT)
        OP_END

        case OP_SGET_OBJECT_VOLATILE: HANDLE_SGET_X(OP_SGET_OBJECT_VOLATILE, "-object-volatile", s8,
                                                    _AS_OBJECT)
        OP_END

        case OP_SPUT_OBJECT_VOLATILE: HANDLE_SPUT_X(OP_SPUT_OBJECT_VOLATILE, "-object-volatile",
                                                    _AS_OBJECT)
        OP_END
        default:
            LOG_E("unknown opcode 0x%02x\n", curOp);
            dvmThrowRuntimeException("unknown opcode");
            return;
    }


    bail:
    delete vmMethodToCall;
    *pResult = retval;
    return;

/* File: c/gotoTargets.cpp */
/*
 * C footer.  This has some common code shared by the various targets.
 */

/*
 * Everything from here on is a "goto target".  In the basic interpreter
 * we jump into these targets and then jump directly to the handler for
 * next instruction.  Here, these are subroutines that return to the caller.
 */

    GOTO_TARGET(filledNewArray, bool methodCallRange, bool)
    {
        jclass arrayClass;
        jarray newArray;
        char typeCh;
        u4 arg5;

        ref = FETCH(1);             /* class ref */
        vdst = FETCH(2);            /* first 4 regs -or- range base */

        if (methodCallRange) {
            vsrc1 = INST_AA(inst);  /* #of elements */
            arg5 = -1;              /* silence compiler warning */
            LOG_D("|filled-new-array-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            arg5 = static_cast<u4>(INST_A(inst));
            vsrc1 = INST_B(inst);   /* #of elements */
            LOG_D("|filled-new-array args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1, ref, vdst, arg5);
        }

        /*
        * Resolve the array class.
        */
        arrayClass = dvmResolveClass(curMethod, ref);
        if (arrayClass == nullptr) {
            GOTO_bail();
        }

        const string descriptor = getClassDescriptorByJClass(arrayClass);
        /* verifier guarantees this is an array class */
        assert(descriptor[0] == '[');

        /*
         * Create an array of the specified type.
         */
        LOG_D("+++ filled-new-array type is '%s'", descriptor.data());
        typeCh = descriptor[1];
        if (typeCh == 'D' || typeCh == 'J') {
            /* category 2 primitives not allowed */
            dvmThrowRuntimeException("bad filled array req");
            GOTO_bail();
        } else if (typeCh != 'L' && typeCh != '[' && typeCh != 'I') {
            LOG_E("non-int primitives not implemented");
            dvmThrowInternalError(
                    "filled-new-array not implemented for anything but 'int'");
            GOTO_bail();
        }

        if (typeCh == 'L' || typeCh == '[') {
            string tmp = descriptor.data() + 1;
            if (tmp[0] == 'L') {
                tmp = tmp.substr(1, tmp.size() - 1);
            }
            jclass elementClazz = (*env).FindClass(tmp.data());
            jobject *contents = new jobject[vsrc1]();
            /*
             * Copy args.  This may corrupt vsrc1/vdst.
             */
            if (methodCallRange) {
                for (int i = 0; i < vsrc1; i++) {
                    memcpy(contents + i, &(GET_REGISTER_AS_OBJECT(vdst + i)), sizeof(jobject));
                }
            } else {
                assert(vsrc1 <= 5);
                switch (vsrc1) {
                    case 5:
                        memcpy(contents + 4, &(GET_REGISTER_AS_OBJECT(vsrc1 & 0x0f)),
                               sizeof(jobject));
                    case 4:
                        memcpy(contents + 3, &(GET_REGISTER_AS_OBJECT(vdst >> 12)),
                               sizeof(jobject));
                    case 3:
                        memcpy(contents + 2, &(GET_REGISTER_AS_OBJECT((vdst & 0x0f00) >> 8)),
                               sizeof(jobject));
                    case 2:
                        memcpy(contents + 1, &(GET_REGISTER_AS_OBJECT((vdst & 0x00f0) >> 4)),
                               sizeof(jobject));
                    case 1:
                        memcpy(contents, &(GET_REGISTER_AS_OBJECT(vdst & 0x0f)), sizeof(jobject));
                    default:;
                }

            }
            newArray = (*env).NewObjectArray(vsrc1, elementClazz, nullptr);
            for (int i = 0; i < vsrc1; i++) {
                (*env).SetObjectArrayElement(reinterpret_cast<jobjectArray>(newArray), i,
                                             contents[i]);
            }
        } else {
            u4 *contents = new u4[vsrc1]();
            /*
             * Copy args.  This may corrupt vsrc1/vdst.
             */
            if (methodCallRange) {
                for (int i = 0; i < vsrc1; i++) {
                    memcpy(contents + i, &(GET_REGISTER(vdst + i)), sizeof(u4));
                }
            } else {
                assert(vsrc1 <= 5);
                switch (vsrc1) {
                    case 5:
                        memcpy(contents + 4, &(GET_REGISTER(vsrc1 & 0x0f)), sizeof(u4));
                    case 4:
                        memcpy(contents + 3, &(GET_REGISTER(vdst >> 12)), sizeof(u4));
                    case 3:
                        memcpy(contents + 2, &(GET_REGISTER((vdst & 0x0f00) >> 8)), sizeof(u4));
                    case 2:
                        memcpy(contents + 1, &(GET_REGISTER((vdst & 0x00f0) >> 4)), sizeof(u4));
                    case 1:
                        memcpy(contents, &(GET_REGISTER(vdst & 0x0f)), sizeof(u4));
                    default:;
                }

            }
            newArray = (*env).NewIntArray(vsrc1);
            (*env).SetIntArrayRegion(reinterpret_cast<jintArray>(newArray), 0, vsrc1,
                                     reinterpret_cast<const jint *>(contents));
        }

        retval.l = newArray;

    }
    FINISH(3);
    GOTO_TARGET_END

    GOTO_TARGET(invokeSuper, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOG_D("|invoke-super-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            LOG_D("|invoke-super args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        methodToCallType = METHOD_SUPER;
        methodToCall = dvmResolveMethod(curMethod, ref, methodToCallType, &methodToCallClazz);
        if (methodToCall == nullptr) {
            GOTO_bail();
        }

#if defined(SHELL_LOG)
        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        LOG_D("+++ super-virtual=%s.%s", vmMethodToCall->clazzDescriptor, vmMethodToCall->name);
#endif

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


    GOTO_TARGET(invokeVirtual, bool methodCallRange, bool)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        /*
         * The object against which we are executing a method is always
         * in the first argument.
         */
        if (methodCallRange) {
            assert(vsrc1 > 0);
            LOG_D("|invoke-virtual-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);

        } else {
            assert((vsrc1 >> 4) > 0);
            LOG_D("|invoke-virtual args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        /*
         * Resolve the method.  This is the correct method for the static
         * type of the object.  We also verify access permissions here.
         */
        methodToCallType = METHOD_VIRTUAL;
        methodToCall = dvmResolveMethod(curMethod, ref, methodToCallType, &methodToCallClazz);
        if (methodToCall == nullptr) {
            LOG_E("+ unknown method or access denied");
            GOTO_bail();
        }

#if defined(SHELL_LOG)
        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        LOG_D("+++ virtual=%s.%s", vmMethodToCall->clazzDescriptor, vmMethodToCall->name);
#endif

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


    GOTO_TARGET(invokeDirect, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOG_D("|invoke-direct-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            LOG_D("|invoke-direct args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        methodToCallType = METHOD_DIRECT;
        methodToCall = dvmResolveMethod(curMethod, ref, methodToCallType, &methodToCallClazz);

        if (methodToCall == nullptr) {
            LOG_E("+ unknown direct method");     // should be impossible
            GOTO_bail();
        }

#if defined(SHELL_LOG)
        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        LOG_D("+++ direct=%s.%s", vmMethodToCall->clazzDescriptor, vmMethodToCall->name);
#endif

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END



    GOTO_TARGET(invokeStatic, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOG_D("|invoke-static-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            LOG_D("|invoke-static args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        methodToCallType = METHOD_STATIC;
        methodToCall = dvmResolveMethod(curMethod, ref, methodToCallType, &methodToCallClazz);
        if (methodToCall == nullptr) {
            LOG_E("+ unknown method");
            GOTO_bail();
        }

#if defined(SHELL_LOG)
        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        LOG_D("+++ static=%s.%s", vmMethodToCall->clazzDescriptor, vmMethodToCall->name);
#endif

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


    GOTO_TARGET(invokeInterface, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        /*
         * The object against which we are executing a method is always
         * in the first argument.
         */
        if (methodCallRange) {
            assert(vsrc1 > 0);
            LOG_D("|invoke-interface-range args=%d @0x%04x {regs=v%d-v%d}",
                  vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            assert((vsrc1 >> 4) > 0);
            LOG_D("|invoke-interface args=%d @0x%04x {regs=0x%04x %x}",
                  vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }
        /*
         * Given a class and a method index, find the Method* with the
         * actual code we want to execute.
         */
        methodToCallType = METHOD_INTERFACE;
        methodToCall = dvmResolveMethod(curMethod, ref, methodToCallType, &methodToCallClazz);
        if (methodToCall == nullptr) {
            /* impossible in verified DEX, need to check for it in unverified */
            dvmThrowIncompatibleClassChangeError("interface not implemented");
            GOTO_bail();
        }

#if defined(SHELL_LOG)
        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        LOG_D("+++ interface concrete=%s.%s", vmMethodToCall->clazzDescriptor,
              vmMethodToCall->name);
#endif

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


    GOTO_TARGET(invokeMethod, bool methodCallRange, const jmethodID _methodToCall, u2 count,
                u2 regs)
    {
        STUB_HACK(vsrc1 = count;
                          vdst = regs;
                          methodToCall = _methodToCall;)
        jobject thisPtr = nullptr;
        jvalue *vars = nullptr;

        // test: do not arrived in normal
        if ((*env).ExceptionCheck()) {
            GOTO_exceptionThrown();
        } else if (curException != nullptr) {
            (*env).Throw(curException);
            GOTO_exceptionThrown();
        }

        vmMethodToCall = initVmMethodNoCode(methodToCall, vmMethodToCall);
        const char *methodToCallShorty = dexStringById(vmMethodToCall->dexFile,
                                                       vmMethodToCall->protoId->shortyIdx);
        /*
         * Copy args.  This may corrupt vsrc1/vdst.
         */
        int var_i = 0;
        int var_i_reg = 0;
        if (methodCallRange) {
            vars = new jvalue[vsrc1]();
            if (methodToCallType != METHOD_STATIC) {
                thisPtr = GET_REGISTER_AS_OBJECT (vdst);
                var_i_reg = 1;
            }
            for (; var_i_reg < vsrc1; var_i_reg++, var_i++) {
                switch (methodToCallShorty[var_i + 1]) {
                    case 'D':
                    case 'J':
                        vars[var_i].j = GET_REGISTER_WIDE(vdst + var_i_reg);
                        var_i_reg++;
                        break;

                    case 'L':
                        vars[var_i].l = GET_REGISTER_AS_OBJECT(vdst + var_i_reg);
                        break;

                    default:
                        memcpy(vars + var_i, &(GET_REGISTER(vdst + var_i_reg)), sizeof(u4));
                        break;
                }
            }
        } else {
            u4 count = vsrc1 >> 4;
            assert(count <= 5);
            vars = new jvalue[MAX(1, count)]();

            // This version executes fewer instructions but is larger
            // overall.  Seems to be a teensy bit faster.
            assert((vdst >> 16) == 0);  // 16 bits -or- high 16 bits clear
            if (methodToCallType != METHOD_STATIC) {
                thisPtr = GET_REGISTER_AS_OBJECT(vdst & 0x0f);

                var_i_reg = 1;
                vdst >>= 4;
            }
            for (; var_i_reg < count && var_i_reg < 4; var_i_reg++, var_i++, vdst >>= 4) {
                switch (methodToCallShorty[var_i + 1]) {
                    case 'D':
                    case 'J':
                        vars[var_i].j = GET_REGISTER_WIDE(vdst & 0x0f);
                        vdst >>= 4;
                        var_i_reg++;
                        break;

                    case 'L':
                        vars[var_i].l = GET_REGISTER_AS_OBJECT(vdst & 0x0f);
                        break;

                    default:
                        memcpy(vars + var_i, &(GET_REGISTER(vdst & 0x0f)), sizeof(u4));
                        break;
                }
            }
        }

        // call method.
        if (methodToCallType == METHOD_STATIC) {
            switch (methodToCallShorty[0]) {
                case 'I':
                    retval.i = (*env).CallStaticIntMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'Z':
                    retval.z = (*env).CallStaticBooleanMethodA(methodToCallClazz, methodToCall,
                                                               vars);
                    break;

                case 'B':
                    retval.b = (*env).CallStaticByteMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'S':
                    retval.s = (*env).CallStaticShortMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'C':
                    retval.c = (*env).CallStaticCharMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'F':
                    retval.f = (*env).CallStaticFloatMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'L':
                    retval.l = (*env).CallStaticObjectMethodA(methodToCallClazz, methodToCall,
                                                              vars);
                    break;

                case 'J':
                    retval.j = (*env).CallStaticLongMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                case 'D':
                    retval.d = (*env).CallStaticDoubleMethodA(methodToCallClazz, methodToCall,
                                                              vars);
                    break;

                case 'V':
                    (*env).CallStaticVoidMethodA(methodToCallClazz, methodToCall, vars);
                    break;

                default:
                    LOG_E("error method's return type(%s)...", methodToCallShorty);
                    dvmThrowRuntimeException("error type of field... cc");
                    break;
            }
        } else if (methodToCallType == METHOD_SUPER) {
            if (!checkForNull(thisPtr)) {
                GOTO_bail();
            }

            switch (methodToCallShorty[0]) {
                case 'I':
                    retval.i = (*env).CallNonvirtualIntMethodA(thisPtr, methodToCallClazz,
                                                               methodToCall, vars);
                    break;

                case 'Z':
                    retval.z = (*env).CallNonvirtualBooleanMethodA(thisPtr, methodToCallClazz,
                                                                   methodToCall, vars);
                    break;

                case 'B':
                    retval.b = (*env).CallNonvirtualByteMethodA(thisPtr, methodToCallClazz,
                                                                methodToCall, vars);
                    break;

                case 'S':
                    retval.s = (*env).CallNonvirtualShortMethodA(thisPtr, methodToCallClazz,
                                                                 methodToCall, vars);
                    break;

                case 'C':
                    retval.c = (*env).CallNonvirtualCharMethodA(thisPtr, methodToCallClazz,
                                                                methodToCall, vars);
                    break;

                case 'F':
                    retval.f = (*env).CallNonvirtualFloatMethodA(thisPtr, methodToCallClazz,
                                                                 methodToCall, vars);
                    break;

                case 'L':
                    retval.l = (*env).CallNonvirtualObjectMethodA(thisPtr, methodToCallClazz,
                                                                  methodToCall, vars);
                    break;

                case 'J':
                    retval.j = (*env).CallNonvirtualLongMethodA(thisPtr, methodToCallClazz,
                                                                methodToCall, vars);
                    break;

                case 'D':
                    retval.d = (*env).CallNonvirtualDoubleMethodA(thisPtr, methodToCallClazz,
                                                                  methodToCall, vars);
                    break;

                case 'V':
                    (*env).CallNonvirtualVoidMethodA(thisPtr, methodToCallClazz, methodToCall,
                                                     vars
                    );
                    break;

                default:
                    LOG_E("error method's return type(%s)...", methodToCallShorty);
                    dvmThrowRuntimeException("error type of field... cc");
                    break;
            }
        } else {    // METHOD_DIRECT || METHOD_VIRTUAL || METHOD_INTERFACE
            if (!checkForNull(thisPtr)) {
                GOTO_bail();
            }

            switch (methodToCallShorty[0]) {
                case 'I':
                    retval.i = (*env).CallIntMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'Z':
                    retval.z = (*env).CallBooleanMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'B':
                    retval.b = (*env).CallByteMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'S':
                    retval.s = (*env).CallShortMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'C':
                    retval.c = (*env).CallCharMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'F':
                    retval.f = (*env).CallFloatMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'L':
                    retval.l = (*env).CallObjectMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'J':
                    retval.j = (*env).CallLongMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'D':
                    retval.d = (*env).CallDoubleMethodA(thisPtr, methodToCall, vars);
                    break;

                case 'V':
                    (*env).CallVoidMethodA(thisPtr, methodToCall, vars);
                    break;

                default:
                    LOG_E("error method's return type(%s)...", methodToCallShorty);
                    dvmThrowRuntimeException("error type of field... cc");
                    break;
            }
        }
// debug print invoke method
#if defined(SHELL_LOG)
        debugInvokeMethod(methodToCall, retval, vars, vmMethodToCall);
#endif

        delete[] vars;

        if ((*env).ExceptionCheck()) {
            GOTO_exceptionThrown();
        }
        FINISH(3);
    }
    GOTO_TARGET_END

    /*
     * Jump here when the code throws an exception.
     *
     * By the time we get here, the Throwable has been created and the stack
     * trace has been saved off.
     */
    GOTO_TARGET(exceptionThrown)
    {
        int catchRelPC;
        PERIODIC_CHECKS(0);

        curException = (*env).ExceptionOccurred();
        assert(curException != nullptr);
        (*env).ExceptionClear();

        /*
         * We need to unroll to the catch block or the nearest "break"
         * frame.
         *
         * A break frame could indicate that we have reached an intermediate
         * native call, or have gone off the top of the stack and the thread
         * needs to exit.  Either way, we return from here, leaving the
         * exception raised.
         *
         * If we do find a catch block, we want to transfer execution to
         * that point.
         *
         * Note this can cause an exception while resolving classes in
         * the "catch" blocks.
         */
        catchRelPC = dvmFindCatchBlock(curMethod, pc - curInsns, curException);
        if (catchRelPC <= 0) {
            (*env).Throw(curException);
            GOTO_bail();
        }
        LOG_D("dvmFindCatchBlock: %d", catchRelPC);
        pc = curInsns + catchRelPC;
        FINISH(0);
    }
    GOTO_TARGET_END
}

void
dvmCallMethod(JNIEnv *env, jobject instance, const VmMethod *method, jvalue *pResult, ...) {
    // 参数入寄存器
    int verifyCount = 0;
    const char *desc = &(dexStringById(method->dexFile,
                                       method->protoId->shortyIdx)[1]); // [0] is the return type.
    jvalue *reg = new jvalue[method->code->registersSize]();
    jvalue *ins = reg + (method->code->registersSize - method->code->insSize);

    if (!dvmIsStaticMethod(method->accessFlags)) {
        ins->l = instance;
        ins++;
        verifyCount++;
    }

    va_list args;
    va_start(args, pResult);
    while (*desc != '\0') {
        switch (*desc++) {
            case 'D':
            case 'J': {
                ins->j = va_arg(args, jlong);
                ins += 2;
                verifyCount += 2;
                break;
            }

            case 'L': {     /* 'shorty' descr uses L for all refs, incl array */
                ins->l = va_arg(args, jobject);
                ins++;
                verifyCount++;
                break;
            }
            default: {
                /* Z B C S I -- all passed as 32-bit integers */
                ins->i = va_arg(args, jint);
                ins++;
                verifyCount++;
                break;
            }
        }
    }
    va_end(args);

    if (verifyCount != method->code->insSize) {
        LOG_E("desc: %s", dexStringById(method->dexFile, method->protoId->shortyIdx));
        LOG_E("Got vfycount=%d insSize=%d for %s", verifyCount, method->code->insSize,
              method->name);
        GOTO_bail();
    }

    // interpret
    dvmInterpret(env, instance, method, reg, pResult);

    bail:
    free(reg);
}

void dvmThrowArithmeticException(const char *msg) {
    dvmThrowNew("java/lang/ArithmeticException", msg);
}

/*
 * Resolve a string reference.
 *
 * Finding the string is easy.  We need to return a reference to a
 * java/lang/String object, not a bunch of characters, which means the
 * first time we get here we need to create an interned string.
 */
/*
 * Create a UTF-16 version so we can trivially compare it to what's
 * already interned.
 */
jstring dvmResolveString(const VmMethod *method, u4 stringIdx) {
    const DexFile *pDexFile = method->dexFile;
    const DexStringId *pStringId = dexGetStringId(pDexFile, stringIdx);
    const char *stringData = dexGetStringData(pDexFile, pStringId);

    LOG_D("+++ resolving string=%s, referrer is %s", stringData, method->clazzDescriptor);
    return (*env).NewStringUTF(stringData);
}

jclass dvmResolveClass(const VmMethod *method, u4 classIdx) {
    return dvmResolveClass(method, classIdx, nullptr);
}

void dvmThrowClassCastException(jclass actual, jclass desired) {
    string msg = getClassDescriptorByJClass(actual);
    msg += " cannot be cast to ";
    msg += getClassDescriptorByJClass(desired);
    dvmThrowNew("java/lang/ClassCastException", msg.data());
}

void dvmThrowNullPointerException(const char *msg) {
    dvmThrowNew("java/lang/NullPointerException", msg);
}

void dvmThrowNegativeArraySizeException(s4 size) {
    char msgBuf[BUFSIZ];
    sprintf(msgBuf, "%d", size);
    dvmThrowNew("java/lang/NegativeArraySizeException", msgBuf);
}

jclass dvmResolveClass(const VmMethod *method, u4 classIdx, string *clazzNameString) {
    const DexFile *pDexFile = method->dexFile;
    string tmp = dexStringByTypeIdx(pDexFile, classIdx);
    auto tmpLen = tmp.size();
    const char *clazzName = tmp.data();
    if (tmpLen > 0 && tmp[0] == 'L') {
        tmp[tmpLen - 1] = '\0';
        clazzName++;
    }
    if (clazzNameString != nullptr) {
        *clazzNameString = clazzName;
    }
    LOG_D("--- resolving class %s (idx=%u referrer=%s)", clazzName, classIdx,
          method->clazzDescriptor);
    return (*env).FindClass(clazzName);
}


jmethodID dvmResolveMethod(const VmMethod *method, u4 methodIdx, MethodType methodType,
                           jclass *methodToCallClazz) {
    const DexFile *pDexFile = method->dexFile;
    const DexMethodId *pMethodId = dexGetMethodId(pDexFile, methodIdx);
    jclass resClass = dvmResolveClass(method, pMethodId->classIdx);
    if (resClass == nullptr) {
        return nullptr;
    }
    *methodToCallClazz = resClass;
    const char *name = dexStringById(pDexFile, pMethodId->nameIdx);
    LOG_D("--- resolving method=%s (idx=%u referrer=%s)", name, methodIdx, method->clazzDescriptor);
    string sign;
    dvmResolveMethodSign(method, pMethodId, &sign);
    if (methodType == METHOD_STATIC) {
        return (*env).GetStaticMethodID(resClass, name, sign.data());
    }
    return (*env).GetMethodID(resClass, name, sign.data());
}

void
dvmResolveMethodSign(const VmMethod *method, const DexMethodId *pMethodId, string *methodSign) {
    dvmResolveMethodSign(method, dexGetProtoId(method->dexFile, pMethodId->protoIdx), methodSign);
}

bool checkForNull(jobject obj) {
    if (obj == nullptr) {
        dvmThrowNullPointerException(nullptr);
        return false;
    }
    return true;
}


jarray dvmAllocArrayByClass(const s4 length, const VmMethod *method, u4 classIdx) {
    const DexFile *pDexFile = method->dexFile;
    const DexTypeId *pTypeId = dexGetTypeId(pDexFile, classIdx);
    const DexStringId *pStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    string tmp = dexGetStringData(pDexFile, pStringId);
    LOG_D("--- resolving class %s (idx=%u referrer=%s)", tmp.data(), classIdx,
          method->clazzDescriptor);

    /* must be array class */
    assert(tmp[0] == '[');
    jclass elementClazz;
    switch (tmp[1]) {
        case 'I':
            return (*env).NewIntArray(length);

        case 'C':
            return (*env).NewCharArray(length);

        case 'B':
            return (*env).NewByteArray(length);

        case 'Z':
            return (*env).NewBooleanArray(length);

        case 'F':
            return (*env).NewFloatArray(length);

        case 'D':
            return (*env).NewDoubleArray(length);

        case 'S':
            return (*env).NewShortArray(length);

        case 'J':
            return (*env).NewLongArray(length);

        case '[':
            elementClazz = (*env).FindClass(tmp.substr(1, tmp.size() - 1).data());
            if (elementClazz == nullptr) {
                return nullptr;
            }
            return (*env).NewObjectArray(length, elementClazz, nullptr);

        case 'L':
            elementClazz = (*env).FindClass(tmp.substr(2, tmp.size() - 3).data());
            if (elementClazz == nullptr) {
                return nullptr;
            }
            return (*env).NewObjectArray(length, elementClazz, nullptr);

        default:
            LOG_E("Unknown primitive type '%s'", tmp.data() + 1);
            dvmThrowRuntimeException("error type of field... cc");
            return nullptr;
    }


}

bool dvmInterpretHandleFillArrayData(jarray arrayObj, const u2 *arrayData) {
    if (arrayObj == nullptr) {
        dvmThrowNullPointerException(nullptr);
        return false;
    }

    /*
     * Array data table format:
     *  ushort ident = 0x0300   magic value
     *  ushort width            width of each element in the table
     *  uint   size             number of elements in the table
     *  ubyte  data[size*width] table of data values (may contain a single-byte
     *                          padding at the end)
     *
     * Total size is 4+(width * size + 1)/2 16-bit code units.
     */

    if (arrayData[0] != kArrayDataSignature) {
        dvmThrowInternalError("bad array data magic");
        return false;
    }

    u4 size = arrayData[2] | (((u4) arrayData[3]) << 16);
    if (size > (*env).GetArrayLength(arrayObj)) {
        dvmThrowArrayIndexOutOfBoundsException((*env).GetArrayLength(arrayObj), size);
        return false;
    }
    const string descriptor = getClassDescriptorByJClass((*env).GetObjectClass(arrayObj));
    switch (descriptor[1]) {
        case 'I':
            (*env).SetIntArrayRegion(reinterpret_cast<jintArray>(arrayObj), 0, size,
                                     reinterpret_cast<const jint *>(arrayData + 4));
            break;

        case 'C':
            (*env).SetCharArrayRegion(reinterpret_cast<jcharArray>(arrayObj), 0, size,
                                      arrayData + 4);
            break;

        case 'B':
            (*env).SetBooleanArrayRegion(reinterpret_cast<jbooleanArray>(arrayObj), 0, size,
                                         reinterpret_cast<const jboolean *>(arrayData + 4));
            break;

        case 'Z':
            (*env).SetIntArrayRegion(reinterpret_cast<jintArray>(arrayObj), 0, size,
                                     reinterpret_cast<const jint *>(arrayData + 4));
            break;

        case 'F':
            (*env).SetFloatArrayRegion(reinterpret_cast<jfloatArray>(arrayObj), 0, size,
                                       reinterpret_cast<const jfloat *>(arrayData + 4));
            break;

        case 'D':
            (*env).SetDoubleArrayRegion(reinterpret_cast<jdoubleArray>(arrayObj), 0, size,
                                        reinterpret_cast<const jdouble *>(arrayData + 4));
            break;

        case 'S':
            (*env).SetShortArrayRegion(reinterpret_cast<jshortArray>(arrayObj), 0, size,
                                       reinterpret_cast<const jshort *>(arrayData + 4));
            break;

        case 'J':
            (*env).SetLongArrayRegion(reinterpret_cast<jlongArray>(arrayObj), 0, size,
                                      reinterpret_cast<const jlong *>(arrayData + 4));
            break;

        default:
            LOG_E("Unknown primitive type '%c'", descriptor[1]);
            dvmThrowRuntimeException("error type of field... cc");
            return false;
    }
    return true;
}

void dvmThrowInternalError(const char *msg) {
    dvmThrowNew("java/lang/InternalError", msg);
}

void dvmThrowArrayIndexOutOfBoundsException(int length, int index) {
    char msg[BUFSIZ];
    memset(msg, 0, BUFSIZ);
    sprintf(msg, "length=%d; index=%d", length, index);
    dvmThrowNew("java/lang/ArrayIndexOutOfBoundsException", msg);
}

void dvmThrowRuntimeException(const char *msg) {
    dvmThrowNew("java/lang/RuntimeException", msg);
}

s4 dvmInterpretHandlePackedSwitch(const u2 *switchData, s4 testVal) {
    const int kInstrLen = 3;

    /*
     * Packed switch data format:
     *  ushort ident = 0x0100   magic value
     *  ushort size             number of entries in the table
     *  int first_key           first (and lowest) switch case value
     *  int targets[size]       branch targets, relative to switch opcode
     *
     * Total size is (4+size*2) 16-bit code units.
     */
    if (*switchData++ != kPackedSwitchSignature) {
        /* should have been caught by verifier */
        dvmThrowInternalError("bad packed switch magic");
        return kInstrLen;
    }

    u2 size = *switchData++;
    assert(size > 0);

    s4 firstKey = *switchData++;
    firstKey |= (*switchData++) << 16;

    int index = testVal - firstKey;
    if (index < 0 || index >= size) {
        LOG_E("Value %d not found in switch (%d-%d)", testVal, firstKey, firstKey + size - 1);
        return kInstrLen;
    }

    /* The entries are guaranteed to be aligned on a 32-bit boundary;
     * we can treat them as a native int array.
     */
    const s4 *entries = (const s4 *) switchData;
    assert(((u8) entries & 0x3) == 0);
    assert(index >= 0 && index < size);
    return s4FromSwitchData(&entries[index]);
}

s4 dvmInterpretHandleSparseSwitch(const u2 *switchData, s4 testVal) {
    const int kInstrLen = 3;
    u2 size;
    const s4 *keys;
    const s4 *entries;

    /*
     * Sparse switch data format:
     *  ushort ident = 0x0200   magic value
     *  ushort size             number of entries in the table; > 0
     *  int keys[size]          keys, sorted low-to-high; 32-bit aligned
     *  int targets[size]       branch targets, relative to switch opcode
     *
     * Total size is (2+size*4) 16-bit code units.
     */

    if (*switchData++ != kSparseSwitchSignature) {
        /* should have been caught by verifier */
        dvmThrowInternalError("bad sparse switch magic");
        return kInstrLen;
    }

    size = *switchData++;
    assert(size > 0);

    /* The keys are guaranteed to be aligned on a 32-bit boundary;
     * we can treat them as a native int array.
     */
    keys = (const s4 *) switchData;
    assert(((u8) keys & 0x3) == 0);

    /* The entries are guaranteed to be aligned on a 32-bit boundary;
     * we can treat them as a native int array.
     */
    entries = keys + size;
    assert(((u8) entries & 0x3) == 0);

    /*
     * Binary-search through the array of keys, which are guaranteed to
     * be sorted low-to-high.
     */
    int lo = 0;
    int hi = size - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;

        s4 foundVal = s4FromSwitchData(&keys[mid]);
        if (testVal < foundVal) {
            hi = mid - 1;
        } else if (testVal > foundVal) {
            lo = mid + 1;
        } else {
            return s4FromSwitchData(&entries[mid]);
        }
    }

    LOG_E("Value %d not found in switch", testVal);
    return kInstrLen;
}

void dvmThrowIncompatibleClassChangeError(const char *msg) {
    dvmThrowNew("java/lang/IncompatibleClassChangeError", msg);
}

bool dvmCanPutArrayElement(const jobject obj, const jobject arrayObj) {
    const string objectClassDesc = getClassDescriptorByJClass((*env).GetObjectClass(obj));
    const string arrayClassDesc = getClassDescriptorByJClass((*env).GetObjectClass(arrayObj));
    return strcmp(arrayClassDesc.data() + 1, objectClassDesc.data()) == 0;
}

void dvmThrowArrayStoreExceptionIncompatibleElement(const jobject obj, const jobject arrayObj) {
    char msg[BUFSIZ];
    memset(msg, 0, BUFSIZ);
    sprintf(msg, "%s cannot be stored in an array of type %s",
            getClassDescriptorByJClass((*env).GetObjectClass(obj)).data(),
            getClassDescriptorByJClass((*env).GetObjectClass(arrayObj)).data());
    dvmThrowNew("java/lang/ArrayStoreException", msg);
}

bool dvmResolveField(const VmMethod *method, u4 ifieldIdx, jobject obj, s8 *res,
                     const char **ppName) {
    const DexFile *pDexFile = method->dexFile;
    LOG_D("--- resolving field %u (referrer=%s)", ifieldIdx, method->clazzDescriptor);
    const DexFieldId *pFieldId = dexGetFieldId(pDexFile, ifieldIdx);
    jclass resClass = dvmResolveClass(method, pFieldId->classIdx);
    if (resClass == nullptr) {
        LOG_E("can't found jclass");
        return false;
    }
    jfieldID resField = nullptr;
    const char *name = dexStringById(pDexFile, pFieldId->nameIdx);
    const char *sign = dexStringByTypeIdx(pDexFile, pFieldId->typeIdx);
    if (obj == nullptr) {
        resField = (*env).GetStaticFieldID(resClass, name, sign);
    } else {
        resField = (*env).GetFieldID(resClass, name, sign);
    }
    if (resField == nullptr) {
        LOG_E("can't found field id.");
        return false;
    }
    jvalue tmpVal;
    switch (sign[0]) {
        case 'I':
            tmpVal.i = obj == nullptr ? (*env).GetStaticIntField(resClass, resField)
                                      : (*env).GetIntField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'F':
            tmpVal.f = obj == nullptr ? (*env).GetStaticFloatField(resClass, resField)
                                      : (*env).GetFloatField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'Z':
            tmpVal.z = obj == nullptr ? (*env).GetStaticBooleanField(resClass, resField)
                                      : (*env).GetBooleanField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'B':
            tmpVal.b = obj == nullptr ? (*env).GetStaticByteField(resClass, resField)
                                      : (*env).GetBooleanField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'C':
            tmpVal.c = obj == nullptr ? (*env).GetStaticCharField(resClass, resField)
                                      : (*env).GetCharField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'S':
            tmpVal.s = obj == nullptr ? (*env).GetStaticShortField(resClass, resField)
                                      : (*env).GetShortField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'J':
            tmpVal.j = obj == nullptr ? (*env).GetStaticLongField(resClass, resField)
                                      : (*env).GetLongField(obj, resField);
            *res = tmpVal.j;
            break;

        case '[':
        case 'L':
            tmpVal.l = obj == nullptr ? (*env).GetStaticObjectField(resClass, resField)
                                      : (*env).GetObjectField(obj, resField);
            *res = tmpVal.j;
            break;

        case 'D':
            tmpVal.d = obj == nullptr ? (*env).GetStaticDoubleField(resClass, resField)
                                      : (*env).GetDoubleField(obj, resField);
            *res = tmpVal.j;
            break;

        default:
            LOG_E("error type of field...");
            dvmThrowRuntimeException("error type of field... cc");
            return false;
    }
    if (ppName != nullptr) {
        *ppName = name;
    }
    return true;
}

bool dvmResolveSetField(const VmMethod *method, u4 ifieldIdx, jobject obj, u8 res,
                        const char **ppName) {
    const DexFile *pDexFile = method->dexFile;
    LOG_D("--- resolving field %u (referrer=%s)", ifieldIdx, method->clazzDescriptor);
    const DexFieldId *pFieldId = dexGetFieldId(pDexFile, ifieldIdx);
    jclass resClass = dvmResolveClass(method, pFieldId->classIdx);
    if (resClass == nullptr) {
        LOG_E("can't found jclass.");
        return false;
    }
    jfieldID resField = nullptr;
    const char *name = dexStringById(pDexFile, pFieldId->nameIdx);
    const char *sign = dexStringByTypeIdx(pDexFile, pFieldId->typeIdx);
    if (obj == nullptr) {
        resField = (*env).GetStaticFieldID(resClass, name, sign);
    } else {
        resField = (*env).GetFieldID(resClass, name, sign);
    }
    if (resField == nullptr) {
        LOG_E("can't found field id.");
        return false;
    }

    jvalue tmpVal;
    memcpy(&tmpVal, &res, sizeof(res));
    switch (sign[0]) {
        case 'I':
            obj == nullptr ? (*env).SetStaticIntField(resClass, resField, tmpVal.i)
                           : (*env).SetIntField(obj, resField, tmpVal.i);
            break;

        case 'F':
            obj == nullptr ? (*env).SetStaticFloatField(resClass, resField, tmpVal.f)
                           : (*env).SetFloatField(obj, resField, tmpVal.f);
            break;

        case 'Z':
            obj == nullptr ? (*env).SetStaticBooleanField(resClass, resField, tmpVal.z)
                           : (*env).SetBooleanField(obj, resField, tmpVal.z);
            break;

        case 'B':
            obj == nullptr ? (*env).SetStaticByteField(resClass, resField, tmpVal.b)
                           : (*env).SetByteField(obj, resField, tmpVal.b);

            break;

        case 'C':
            obj == nullptr ? (*env).SetStaticCharField(resClass, resField, tmpVal.c)
                           : (*env).SetCharField(obj, resField, tmpVal.c);
            break;

        case 'S':
            obj == nullptr ? (*env).SetStaticShortField(resClass, resField, tmpVal.s)
                           : (*env).SetShortField(obj, resField, tmpVal.s);
            break;

        case 'J':
            obj == nullptr ? (*env).SetStaticLongField(resClass, resField, tmpVal.j)
                           : (*env).SetLongField(obj, resField, tmpVal.j);
            break;

        case '[':
            if (sign[1] == 'L') {
                jobject resFieldJava =
                        (*env).ToReflectedField(resClass, resField,
                                                static_cast<jboolean>(obj == nullptr));
                jclass cField = (*env).GetObjectClass(resFieldJava);
                jmethodID mFieldSet = (*env).GetMethodID(cField, "set",
                                                         "(Ljava/lang/Object;Ljava/lang/Object;)V");
                (*env).CallVoidMethod(resFieldJava, mFieldSet, obj, tmpVal.l);
                break;
            }

            // no break
        case 'L':
            obj == nullptr ? (*env).SetStaticObjectField(resClass, resField, tmpVal.l)
                           : (*env).SetObjectField(obj, resField, tmpVal.l);
            break;

        case 'D':
            obj == nullptr ? (*env).SetStaticDoubleField(resClass, resField, tmpVal.d)
                           : (*env).SetDoubleField(obj, resField, tmpVal.d);
            break;

        default:
            LOG_E("error type of field...");
            dvmThrowRuntimeException("error type of field... cc");
            return false;
    }
    if (ppName != nullptr) {
        *ppName = name;
    }
    return true;
}

#if defined(SHELL_LOG)

void debugWriteDex(const VmMethod *method, const char *path) {
    LOG_D("start writing dex to %s", path);
    ofstream writer(path, ios::binary);
    writer.write(reinterpret_cast<const char *>(method->dexFile->baseAddr),
                 method->dexFile->pHeader->fileSize);
    writer.flush();
    writer.close();
    LOG_D("finish dex to %s", path);
}

void debugInvokeMethod(jmethodID jniMethod, const jvalue retVal, const jvalue *vars,
                       VmMethod *vmMethod) {
    const auto *method = initVmMethodNoCode(jniMethod, vmMethod);
    const char *shorty = dexStringById(method->dexFile, method->protoId->shortyIdx);
    LOG_D("invoke method: %s .%s    %s", method->clazzDescriptor, method->name, shorty);
    switch (shorty[0]) {
        case 'I':
            LOG_D("return value (int): %d", retVal.i);
            break;

        case 'Z':
            retVal.z ? LOG_D("return value (bool): true") : LOG_D("return value (bool): false");
            break;

        case 'B':
            LOG_D("return value (byte): 0x%02x", retVal.b);
            break;

        case 'S':
            LOG_D("return value (short): %d", retVal.s);
            break;

        case 'C':
            LOG_D("return value (char): %c", retVal.c);
            break;

        case 'F':
            LOG_D("return value (float): %f", retVal.f);
            break;

        case 'L':
            LOG_D("return value (object): %p", retVal.l);
            break;

        case 'J':
            LOG_D("return value (long): %ld", retVal.j);
            break;

        case 'D':
            LOG_D("return value (double): %lf", retVal.d);
            break;

        case 'V':
            LOG_D("return value (void)");
            break;

        default:
            LOG_E("error method's return type(%s)...", shorty);
            debugWriteDex(method, (getDataDir(env) + "/classes.dex").data());
            assert(false);
            break;
    }

    for (int var_i = 0; shorty[var_i + 1] != '\0'; var_i++) {
        switch (shorty[var_i + 1]) {
            case 'I':
                LOG_D("var(0x%02x) value (int): %d", var_i, vars[var_i].i);
                break;

            case 'Z':
                vars[var_i].z ? LOG_D("var(0x%02x) value (bool): true", var_i)
                              : LOG_D("var(0x%02x) value (bool): false", var_i);
                break;

            case 'B':
                LOG_D("var(0x%02x) value (byte): 0x%02x", var_i, vars[var_i].b);
                break;

            case 'S':
                LOG_D("var(0x%02x) value (short): %d", var_i, vars[var_i].s);
                break;

            case 'C':
                LOG_D("var(0x%02x) value (char): %c", var_i, vars[var_i].c);
                break;

            case 'F':
                LOG_D("var(0x%02x) value (float): %f", var_i, vars[var_i].f);
                break;

            case 'L':
                LOG_D("var(0x%02x) value (object): %p", var_i, vars[var_i].l);
                break;

            case 'J':
                LOG_D("var(0x%02x) value (long): %ld", var_i, vars[var_i].j);
                break;

            case 'D':
                LOG_D("var(0x%02x) value (double): %lf", var_i, vars[var_i].d);
                break;

            default:
                LOG_E("error method's param type(%s)...", shorty);
                debugWriteDex(method, (getDataDir(env) + "/classes.dex").data());
                assert(false);
                break;
        }
    }
}

#endif

int dvmFindCatchBlock(const VmMethod *method, size_t pcOff, jobject exception) {
    const DexFile *pDexFile = method->dexFile;
    const CodeItemData *pCodeItemData = method->code;
    const auto *pDexTry = reinterpret_cast<const DexTry *>(pCodeItemData->triesAndHandlersBuf);
    const u1 *catchHandlerList = pCodeItemData->triesAndHandlersBuf +
                                 pCodeItemData->triesSize * sizeof(DexTry);

    const DexTry *pRetDexTry = nullptr;
    int catchOff = 0;
    // find try
    for (int i = 0; i < pCodeItemData->triesSize; i++, pDexTry++) {
        if (pDexTry->startAddr <= pcOff && pcOff < pDexTry->startAddr + pDexTry->insnCount &&
            (pRetDexTry == nullptr || (pRetDexTry->startAddr < pDexTry->startAddr))) {
            const u1 *pCatchHandler = catchHandlerList + pDexTry->handlerOff;
            int size = readSignedLeb128(&pCatchHandler);
            // find catch
            bool catchFlag = false;
            for (int j = 0; j < abs(size); j++) {
                u4 type_idx = static_cast<u4>(readUnsignedLeb128(&pCatchHandler));
                int address = readUnsignedLeb128(&pCatchHandler);

                const DexTypeId *pDexTypeId = dexGetTypeId(pDexFile, type_idx);
                const DexStringId *pDexStringId = dexGetStringId(pDexFile,
                                                                 pDexTypeId->descriptorIdx);
                string tmp = dexGetStringData(pDexFile, pDexStringId);
                if (tmp.size() < 3) {
                    continue;
                }
                tmp = tmp.substr(1, tmp.size() - 2);
                jclass catchExceptionClazz = (*env).FindClass(tmp.data());
                assert(catchExceptionClazz != nullptr);
                if ((*env).IsInstanceOf(exception, catchExceptionClazz)) {
                    // may catch the exception
                    pRetDexTry = pDexTry;
                    catchOff = address;
                    catchFlag = true;
                    break;
                }
            }
            if (!catchFlag && size <= 0) {
                catchOff = readUnsignedLeb128(&pCatchHandler);
            }
        }
    }
    return catchOff;
}

/*
 * Throw an exception for a problem identified by the verifier.
 *
 * This is used by the invoke-verification-error instruction.  It always
 * throws an exception.
 *
 * "kind" indicates the kind of failure encountered by the verifier.  It
 * has two parts, an error code and an indication of the reference type.
 */
void dvmThrowVerificationError(const VmMethod *method, int kind, int ref) {
    int errorPart = kind & ~(0xff << kVerifyErrorRefTypeShift);
    int errorRefPart = kind >> kVerifyErrorRefTypeShift;
    auto errorKind = static_cast<VerifyError>(errorPart);
    auto refType = static_cast<VerifyErrorRefType>(errorRefPart);
    jclass exceptionClass = (*env).FindClass("java/lang/VerifyError");
    string msg;

    switch (errorKind) {
        case VERIFY_ERROR_NO_CLASS:
            exceptionClass = (*env).FindClass("java/lang/NoClassDefFoundError");
            msg = classNameFromIndex(method, ref, refType, 0);
            break;
        case VERIFY_ERROR_NO_FIELD:
            exceptionClass = (*env).FindClass("java/lang/NoSuchFieldError");
            msg = fieldNameFromIndex(method, ref, refType, 0);
            break;
        case VERIFY_ERROR_NO_METHOD:
            exceptionClass = (*env).FindClass("java/lang/NoSuchMethodError");
            msg = methodNameFromIndex(method, ref, refType, 0);
            break;
        case VERIFY_ERROR_ACCESS_CLASS:
            exceptionClass = (*env).FindClass("java/lang/IllegalAccessError");
            msg = classNameFromIndex(method, ref, refType, kThrowShow_accessFromClass);
            break;
        case VERIFY_ERROR_ACCESS_FIELD:
            exceptionClass = (*env).FindClass("java/lang/IllegalAccessError");
            msg = fieldNameFromIndex(method, ref, refType, kThrowShow_accessFromClass);
            break;
        case VERIFY_ERROR_ACCESS_METHOD:
            exceptionClass = (*env).FindClass("java/lang/IllegalAccessError");
            msg = methodNameFromIndex(method, ref, refType, kThrowShow_accessFromClass);
            break;
        case VERIFY_ERROR_CLASS_CHANGE:
            exceptionClass = (*env).FindClass("java/lang/IncompatibleClassChangeError");
            msg = classNameFromIndex(method, ref, refType, 0);
            break;
        case VERIFY_ERROR_INSTANTIATION:
            exceptionClass = (*env).FindClass("java/lang/InstantiationError");
            msg = classNameFromIndex(method, ref, refType, 0);
            break;

        case VERIFY_ERROR_GENERIC:
            /* generic VerifyError; use default exception, no message */
            break;
        case VERIFY_ERROR_NONE:
            /* should never happen; use default exception */
            assert(false);
            msg = "weird - no error specified";
            break;

            /* no default clause -- want warning if enum updated */
    }

    (*env).ThrowNew(exceptionClass, msg.data());
}

string dvmHumanReadableDescriptor(const char *descriptor) {
    // Count the number of '['s to get the dimensionality.
    const char *c = descriptor;
    size_t dim = 0;
    while (*c == '[') {
        dim++;
        c++;
    }

    // Reference or primitive?
    if (*c == 'L') {
        // "[[La/b/C;" -> "a.b.C[][]".
        c++; // Skip the 'L'.
    } else {
        // "[[B" -> "byte[][]".
        // To make life easier, we make primitives look like unqualified
        // reference types.
        switch (*c) {
            case 'B':
                c = "byte;";
                break;
            case 'C':
                c = "char;";
                break;
            case 'D':
                c = "double;";
                break;
            case 'F':
                c = "float;";
                break;
            case 'I':
                c = "int;";
                break;
            case 'J':
                c = "long;";
                break;
            case 'S':
                c = "short;";
                break;
            case 'Z':
                c = "boolean;";
                break;
            default:
                return descriptor;
        }
    }

    // At this point, 'c' is a string of the form "fully/qualified/Type;"
    // or "primitive;". Rewrite the type with '.' instead of '/':
    string result;
    const char *p = c;
    while (*p != ';') {
        char ch = *p++;
        if (ch == '/') {
            ch = '.';
        }
        result.push_back(ch);
    }
    // ...and replace the semicolon with 'dim' "[]" pairs:
    while (dim--) {
        result += "[]";
    }
    return result;
}


string classNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags) {
    const DexFile *pDexFile = method->dexFile;
    if (refType == VERIFY_ERROR_REF_FIELD) {
        /* get class ID from field ID */
        const DexFieldId *pFieldId = dexGetFieldId(pDexFile, static_cast<u4>(ref));
        ref = pFieldId->classIdx;
    } else if (refType == VERIFY_ERROR_REF_METHOD) {
        /* get class ID from method ID */
        const DexMethodId *pMethodId = dexGetMethodId(pDexFile, static_cast<u4>(ref));
        ref = pMethodId->classIdx;
    }

    const char *className = dexStringByTypeIdx(pDexFile, static_cast<u4>(ref));
    string dotClassName(dvmHumanReadableDescriptor(className));
    if (flags == 0) {
        return dotClassName;
    }

    string result;
    if ((flags & kThrowShow_accessFromClass) != 0) {
        result += "tried to access class " + dotClassName;
        result += " from class " + dvmHumanReadableDescriptor(method->clazzDescriptor);
    } else {
        assert(false);      // should've been caught above
    }

    return result;
}

string fieldNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags) {
    if (refType != VERIFY_ERROR_REF_FIELD) {
        LOG_E("Expected ref type %d, got %d", VERIFY_ERROR_REF_FIELD, refType);
        return nullptr;    /* no message */
    }

    const DexFile *pDexFile = method->dexFile;
    const DexFieldId *pFieldId = dexGetFieldId(pDexFile, static_cast<u4>(ref));
    const char *className = dexStringByTypeIdx(pDexFile, pFieldId->classIdx);
    const char *fieldName = dexStringById(pDexFile, pFieldId->nameIdx);

    string dotName(dvmHumanReadableDescriptor(className));

    if ((flags & kThrowShow_accessFromClass) != 0) {
        string result;
        result += "tried to access field ";
        result += dotName + "." + fieldName;
        result += " from class ";
        result += dvmHumanReadableDescriptor(method->clazzDescriptor);
        return result;
    }
    return dotName + "." + fieldName;
}

string methodNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags) {
    if (refType != VERIFY_ERROR_REF_METHOD) {
        LOG_E("Expected ref type %d, got %d", VERIFY_ERROR_REF_METHOD, refType);
        return nullptr;    /* no message */
    }

    const DexFile *pDexFile = method->dexFile;
    const DexMethodId *pMethodId = dexGetMethodId(pDexFile, static_cast<u4>(ref));
    const char *className = dexStringByTypeIdx(pDexFile, pMethodId->classIdx);
    const char *methodName = dexStringById(pDexFile, pMethodId->nameIdx);

    string dotName(dvmHumanReadableDescriptor(className));

    if ((flags & kThrowShow_accessFromClass) != 0) {
        string desc;
        dvmResolveMethodSign(method, method->protoId, &desc);
        string result;
        result += "tried to access method ";
        result += dotName + "." + methodName + ":" + desc;
        result += " from class " + dvmHumanReadableDescriptor(method->clazzDescriptor);
        return result;
    }
    return dotName + "." + methodName;
}

void
dvmResolveMethodSign(const VmMethod *method, const DexProtoId *pDexProtoId, string *methodSign) {
    const DexFile *pDexFile = method->dexFile;
    *methodSign = "(";
    const DexTypeList *pTypeList = dexGetProtoParameters(pDexFile, pDexProtoId);
    const DexTypeItem *pDexTypeItem = nullptr;
    const DexTypeId *pTypeId = nullptr;
    const DexStringId *pDexStringId = nullptr;
    if (pTypeList != nullptr) {
        for (u4 i = 0; i < pTypeList->size; i++) {
            pDexTypeItem = dexGetTypeItem(pTypeList, i);
            pTypeId = dexGetTypeId(pDexFile, pDexTypeItem->typeIdx);
            pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
            *methodSign += dexGetStringData(pDexFile, pDexStringId);
        }
    }
    *methodSign += ")";
    pTypeId = dexGetTypeId(pDexFile, pDexProtoId->returnTypeIdx);
    pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    const char *returnDescriptor = dexGetStringData(pDexFile, pDexStringId);
    *methodSign += returnDescriptor;
    LOG_D("--- resolving method sign %s", methodSign->data());
}

const VmMethod *initVmMethod(jmethodID jniMethod) {
    VmMethod *method = initVmMethodNoCode(jniMethod);
    method->code = getCodeItem(method);
    return method;
}

const CodeItemData *getCodeItem(const VmMethod *method) {
    // (sign + method_name + class_name).reverse
    string key;
    dvmResolveMethodSign(method, method->protoId, &key);
    key += method->name;
    key += method->clazzDescriptor;
    reverse(key.begin(), key.end());
    return getCodeItem(key);
}


const DexFile *initDexFileInArt(const uint8_t *buf, size_t size) {
    auto *pDexHeader = (DexHeader *) buf;
    LOG_D("dex marge: %s", buf);
    if (strncmp((char *) buf, "cdex", 4) == 0) {
        auto* pOdexHeader = (DexOptHeader*)buf;
        pDexHeader = (DexHeader *)(buf + pOdexHeader->dexOffset);
    }
    auto *pDexFile = new DexFile();
    pDexFile->baseAddr = (u1 *)pDexHeader;
    pDexFile->pHeader = pDexHeader;
    pDexFile->pStringIds = (const DexStringId *) (pDexFile->baseAddr +
                                                  pDexHeader->stringIdsOff);
    pDexFile->pTypeIds = (const DexTypeId *) (pDexFile->baseAddr + pDexHeader->typeIdsOff);
    pDexFile->pFieldIds = (const DexFieldId *) (pDexFile->baseAddr + pDexHeader->fieldIdsOff);
    pDexFile->pMethodIds = (const DexMethodId *) (pDexFile->baseAddr +
                                                  pDexHeader->methodIdsOff);
    pDexFile->pProtoIds = (const DexProtoId *) (pDexFile->baseAddr + pDexHeader->protoIdsOff);
    pDexFile->pClassDefs = (const DexClassDef *) (pDexFile->baseAddr +
                                                  pDexHeader->classDefsOff);
    pDexFile->pLinkData = (const DexLink *) (pDexFile->baseAddr + pDexHeader->linkOff);
    // check dex file size
    if (pDexHeader->fileSize != size) {
        LOG_E("ERROR: stored file size (%d) != expected (%ld)", pDexHeader->fileSize, size);
    }
    return pDexFile;
}

VmMethod *initVmMethodNoCode(jmethodID jniMethod, VmMethod *pVmMethod) {
    assert(jniMethod != nullptr);
    VmMethod *method;
    if (pVmMethod == nullptr) {
        method = new VmMethod();
    } else {
        method = pVmMethod;
    }

    if (isArtVm(env)) {
        void *artMethod = jniMethod;
        ArtClass *artClass = nullptr;
        void *artDexCache = nullptr;
        ArtDexFile *artDexFile = nullptr;
        const DexMethodId *pDexMethodId = nullptr;

        switch (android_get_device_api_level()) {
            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                artClass = (ArtClass *) (uint64_t) ((ArtMethod_21_22 *) artMethod)->declaring_class;
                artDexCache = (void *) (uint64_t) artClass->dex_cache;
                artDexFile = (ArtDexFile *) ((ArtDexCache_21_23 *) artDexCache)->dex_file;
                method->dexFile = initDexFileInArt(artDexFile->begin, artDexFile->size);
                pDexMethodId = dexGetMethodId(method->dexFile,
                                              ((ArtMethod_21_22 *) artMethod)->dex_method_index);
                method->protoId = dexGetProtoId(method->dexFile, pDexMethodId->protoIdx);
                method->accessFlags = ((ArtMethod_21_22 *) artMethod)->access_flags;
                method->clazzDescriptor = dexStringByTypeIdx(method->dexFile,
                                                             pDexMethodId->classIdx);
                method->name = dexStringById(method->dexFile, pDexMethodId->nameIdx);
                break;

            case __ANDROID_API_M__:
                artClass = (ArtClass *) (uint64_t) ((ArtMethod_23 *) artMethod)->declaring_class;
                artDexCache = (void *) (uint64_t) artClass->dex_cache;
                artDexFile = (ArtDexFile *) ((ArtDexCache_21_23 *) artDexCache)->dex_file;
                method->dexFile = initDexFileInArt(artDexFile->begin, artDexFile->size);
                pDexMethodId = dexGetMethodId(method->dexFile,
                                              ((ArtMethod_23 *) artMethod)->dex_method_index);
                method->protoId = dexGetProtoId(method->dexFile, pDexMethodId->protoIdx);
                method->accessFlags = ((ArtMethod_23 *) artMethod)->access_flags;
                method->clazzDescriptor = dexStringByTypeIdx(method->dexFile,
                                                             pDexMethodId->classIdx);
                method->name = dexStringById(method->dexFile, pDexMethodId->nameIdx);
                break;

            case __ANDROID_API_O__:
            case __ANDROID_API_O_MR1__:
            case __ANDROID_API_P__:
                artClass = (ArtClass *) (uint64_t) ((ArtMethod_26_28 *) artMethod)->declaring_class;
                artDexCache = (void *) (uint64_t) artClass->dex_cache;
                artDexFile = (ArtDexFile *) ((ArtDexCache_26_28 *) artDexCache)->dex_file;
                method->dexFile = initDexFileInArt(artDexFile->begin, artDexFile->size);
                pDexMethodId = dexGetMethodId(method->dexFile,
                                              ((ArtMethod_26_28 *) artMethod)->dex_method_index);
                method->protoId = dexGetProtoId(method->dexFile, pDexMethodId->protoIdx);
                method->accessFlags = ((ArtMethod_26_28 *) artMethod)->access_flags;
                method->clazzDescriptor = dexStringByTypeIdx(method->dexFile,
                                                             pDexMethodId->classIdx);
                method->name = dexStringById(method->dexFile, pDexMethodId->nameIdx);
                break;

            default:
                LOG_E("error android api level: %d", android_get_device_api_level());
                assert(false);
        }
    } else {
        const auto *dvmMethod = reinterpret_cast<const Method *>(jniMethod);
        method->dexFile = dvmMethod->clazz->pDvmDex->pDexFile;
        method->protoId = dexGetProtoId(method->dexFile, dvmMethod->prototype.protoIdx);
        method->accessFlags = dvmMethod->accessFlags;
        method->clazzDescriptor = dvmMethod->clazz->descriptor;
        method->name = dvmMethod->name;
    }
    return method;
}

const string getClassDescriptorByJClass(jclass clazz) {
    jclass cClass = (*env).FindClass("java/lang/Class");
    jmethodID mGetCanonicalName = (*env).GetMethodID(cClass, "getCanonicalName",
                                                     "()Ljava/lang/String;");
    jstring utfString = (jstring) (*env).CallObjectMethod(clazz, mGetCanonicalName);
    string javaDesc = (*env).GetStringUTFChars(utfString, JNI_FALSE);
    string retValue;
    const char *pS = javaDesc.data();
    const char *pE = pS + javaDesc.size() - 1;
    while (pS <= pE && *pE == ']') {
        retValue += '[';
        pE -= 2;
    }
    assert(pS < pE);
    if (strncmp(pS, "int", 3) == 0) {
        retValue += 'I';
    } else if (strncmp(pS, "float", 5) == 0) {
        retValue += 'F';
    } else if (strncmp(pS, "char", 4) == 0) {
        retValue += 'C';
    } else if (strncmp(pS, "short", 5) == 0) {
        retValue += 'S';
    } else if (strncmp(pS, "boolean", 7) == 0) {
        retValue += 'Z';
    } else if (strncmp(pS, "byte", 4) == 0) {
        retValue += 'B';
    } else if (strncmp(pS, "long", 4) == 0) {
        retValue += 'J';
    } else if (strncmp(pS, "double", 6) == 0) {
        retValue += 'D';
    } else {
        retValue += 'L';
    }
    return retValue;
}

void deleteVmMethod(const VmMethod *method) {
    if (method == nullptr) {
        return;
    }
    if (isArtVm(env)) {
        delete method->dexFile;
    }
    delete method;
}