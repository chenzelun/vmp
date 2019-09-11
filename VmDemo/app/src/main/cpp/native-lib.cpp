
#include <math.h>
#include "native-lib.h"
#include "Utils.h"

// auto by manifest
const char *MAIN_ACTIVITY = "com/chend/vmdemo/MainActivity";

void
dvmInterpret(JNIEnv *env, jobject instance, const Method *curMethod, u4 *reg, const u2 *curInsns,
             jvalue *pResult) {

    /* core state */
    const u2 *pc;               // program counter
    u2 inst;                    // current instruction
    /* instruction decoding */
    u4 ref;                     // 16 or 32-bit quantity fetched directly
    u2 vsrc1, vsrc2, vdst;      // usually used for register indexes
    jvalue retval;

    jthrowable curException = nullptr;

    bool methodCallRange = false;
    jmethodID methodToCall = nullptr;
    MethodType methodToCallType = METHOD_UNKNOWN;
    jclass methodToCallClazz = nullptr;

    pc = curInsns;

    int curOp;

    FINISH(0);                  /* fetch and execute first instruction */


    NEXT_INSNS:


    switch (curOp) {
        // 0x00-0x0c
        case OP_NOP:
            HANDLE_OPCODE(OP_NOP) FINISH(1);
            OP_END

        case OP_MOVE:
            HANDLE_OPCODE(OP_MOVE /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOGD("|move%s v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(1);
            OP_END

        case OP_MOVE_FROM16:
            HANDLE_OPCODE(OP_MOVE_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|move%s/from16 v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_FROM16) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(2);
            OP_END

        case OP_MOVE_16:
            HANDLE_OPCODE(OP_MOVE_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOGD("|move%s/16 v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_16) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(3);
            OP_END

        case OP_MOVE_WIDE:
            HANDLE_OPCODE(OP_MOVE_WIDE /*vA, vB*/)
            /* IMPORTANT: must correctly handle overlapping registers, e.g. both
            * "move-wide v6, v7" and "move-wide v7, v6" */
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOGD("|move-wide v%d,v%d %s(v%d=0x%08llx)", vdst, vsrc1,
                 kSpacing + 5, vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(1);
            OP_END

        case OP_MOVE_WIDE_FROM16:
            HANDLE_OPCODE(OP_MOVE_WIDE_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|move-wide/from16 v%d,v%d  (v%d=0x%08llx)", vdst, vsrc1,
                 vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(2);
            OP_END

        case OP_MOVE_WIDE_16:
            HANDLE_OPCODE(OP_MOVE_WIDE_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOGD("|move-wide/16 v%d,v%d %s(v%d=0x%08llx)", vdst, vsrc1,
                 kSpacing + 8, vdst, GET_REGISTER_WIDE(vsrc1));
            SET_REGISTER_WIDE(vdst, GET_REGISTER_WIDE(vsrc1));
            FINISH(3);
            OP_END

        case OP_MOVE_OBJECT:
            HANDLE_OPCODE(OP_MOVE_OBJECT /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOGD("|move%s v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(1);
            OP_END


        case OP_MOVE_OBJECT_FROM16:
            HANDLE_OPCODE(OP_MOVE_OBJECT_FROM16 /*vAA, vBBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|move%s/from16 v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_FROM16) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(2);
            OP_END


        case OP_MOVE_OBJECT_16:
            HANDLE_OPCODE(OP_MOVE_OBJECT_16 /*vAAAA, vBBBB*/)
            vdst = FETCH(1);
            vsrc1 = FETCH(2);
            LOGD("|move%s/16 v%d,v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_16) ? "" : "-object", vdst, vsrc1,
                 kSpacing, vdst, GET_REGISTER(vsrc1));
            SET_REGISTER(vdst, GET_REGISTER(vsrc1));
            FINISH(3);
            OP_END


        case OP_MOVE_RESULT:
            HANDLE_OPCODE(OP_MOVE_RESULT /*vAA*/)
            vdst = INST_AA(inst);
            LOGD("|move-result%s v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_RESULT) ? "" : "-object",
                 vdst, kSpacing + 4, vdst, retval.i);
            SET_REGISTER(vdst, retval.i);
            FINISH(1);
            OP_END

        case OP_MOVE_RESULT_WIDE:
            HANDLE_OPCODE(OP_MOVE_RESULT_WIDE /*vAA*/)
            vdst = INST_AA(inst);
            LOGD("|move-result-wide v%d %s(0x%08llx)", vdst, kSpacing, retval.j);
            SET_REGISTER_WIDE(vdst, retval.j);
            FINISH(1);
            OP_END

        case OP_MOVE_RESULT_OBJECT:
            HANDLE_OPCODE(OP_MOVE_RESULT_OBJECT /*vAA*/)
            vdst = INST_AA(inst);
            LOGD("|move-result%s v%d %s(v%d=0x%08x)",
                 (INST_INST(inst) == OP_MOVE_RESULT) ? "" : "-object",
                 vdst, kSpacing + 4, vdst, retval.i);
            SET_REGISTER(vdst, retval.i);
            FINISH(1);
            OP_END

        case OP_MOVE_EXCEPTION:
            HANDLE_OPCODE(OP_MOVE_EXCEPTION /*vAA*/)
            vdst = INST_AA(inst);
            LOGD("|move-exception v%d", vdst);
            assert(curException != NULL);
            SET_REGISTER(vdst, (u4) curException);
            curException = nullptr;
            FINISH(1);
            OP_END

            // 0x0e-1c
        case OP_RETURN_VOID:
            HANDLE_OPCODE(OP_RETURN_VOID /**/)
            LOGD("|return-void");
            OP_END
            break;

        case OP_RETURN:
            HANDLE_OPCODE(OP_RETURN /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOGD("|return%s v%d", (INST_INST(inst) == OP_RETURN) ? "" : "-object", vsrc1);
            retval.i = GET_REGISTER(vsrc1);
            OP_END
            break;

        case OP_RETURN_WIDE:
            HANDLE_OPCODE(OP_RETURN_WIDE /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOGD("|return-wide v%d", vsrc1);
            retval.j = GET_REGISTER_WIDE(vsrc1);
            OP_END
            break;

        case OP_RETURN_OBJECT:
            HANDLE_OPCODE(OP_RETURN_OBJECT /*vAA*/)
            vsrc1 = INST_AA(inst);
            LOGD("|return%s v%d",
                 (INST_INST(inst) == OP_RETURN) ? "" : "-object", vsrc1);
            retval.i = GET_REGISTER(vsrc1);
            OP_END
            break;

        case OP_CONST_4:
            HANDLE_OPCODE(OP_CONST_4 /*vA, #+B*/) {
            s4 tmp;

            vdst = static_cast<u2>(INST_A(inst));
            tmp = (s4) (INST_B(inst) << 28) >> 28;  // sign extend 4-bit value
            LOGD("|const/4 v%d,#0x%02x", vdst, (s4) tmp);
            SET_REGISTER(vdst, tmp);
        }
            FINISH(1);
            OP_END

        case OP_CONST_16:
            HANDLE_OPCODE(OP_CONST_16 /*vAA, #+BBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|const/16 v%d,#0x%04x", vdst, (s2) vsrc1);
            SET_REGISTER(vdst, (s2) vsrc1);
            FINISH(2);
            OP_END

        case OP_CONST:
            HANDLE_OPCODE(OP_CONST /*vAA, #+BBBBBBBB*/) {
            u4 tmp;

            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOGD("|const v%d,#0x%08x", vdst, tmp);
            SET_REGISTER(vdst, tmp);
        }
            FINISH(3);
            OP_END

        case OP_CONST_HIGH16:
            HANDLE_OPCODE(OP_CONST_HIGH16 /*vAA, #+BBBB0000*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|const/high16 v%d,#0x%04x0000", vdst, vsrc1);
            SET_REGISTER(vdst, vsrc1 << 16);
            FINISH(2);
            OP_END

        case OP_CONST_WIDE_16:
            HANDLE_OPCODE(OP_CONST_WIDE_16 /*vAA, #+BBBB*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|const-wide/16 v%d,#0x%04x", vdst, (s2) vsrc1);
            SET_REGISTER_WIDE(vdst, (s2) vsrc1);
            FINISH(2);
            OP_END

        case OP_CONST_WIDE_32:
            HANDLE_OPCODE(OP_CONST_WIDE_32 /*vAA, #+BBBBBBBB*/) {
            u4 tmp;

            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOGD("|const-wide/32 v%d,#0x%08x", vdst, tmp);
            SET_REGISTER_WIDE(vdst, (s4) tmp);
        }
            FINISH(3);
            OP_END

        case OP_CONST_WIDE:
            HANDLE_OPCODE(OP_CONST_WIDE /*vAA, #+BBBBBBBB BBBBBBBB*/) {
            u8 tmp;

            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u8) FETCH(2) << 16;
            tmp |= (u8) FETCH(3) << 32;
            tmp |= (u8) FETCH(4) << 48;
            LOGD("|const-wide v%d,#0x%08llx", vdst, tmp);
            SET_REGISTER_WIDE(vdst, tmp);
        }
            FINISH(5);
            OP_END

        case OP_CONST_WIDE_HIGH16:
            HANDLE_OPCODE(OP_CONST_WIDE_HIGH16 /*vAA, #+BBBB000000000000*/)
            vdst = INST_AA(inst);
            vsrc1 = FETCH(1);
            LOGD("|const-wide/high16 v%d,#0x%04x000000000000", vdst, vsrc1);
            SET_REGISTER_WIDE(vdst, ((u8) vsrc1) << 48);
            FINISH(2);
            OP_END

        case OP_CONST_STRING:
            HANDLE_OPCODE(OP_CONST_STRING /*vAA, string@BBBB*/) {
            jstring strObj = nullptr;

            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOGD("|const-string v%d string@0x%04x", vdst, ref);
            // get string from dex by stringId
            strObj = dvmResolveString(curMethod->clazz, ref);
            assert(strObj != nullptr);
            SET_REGISTER(vdst, (u4) strObj);
        }
            FINISH(2);
            OP_END

        case OP_CONST_STRING_JUMBO:
            HANDLE_OPCODE(OP_CONST_STRING_JUMBO /*vAA, string@BBBBBBBB*/) {
            jstring strObj = nullptr;
            u4 tmp;

            vdst = INST_AA(inst);
            tmp = FETCH(1);
            tmp |= (u4) FETCH(2) << 16;
            LOGD("|const-string/jumbo v%d string@0x%08x", vdst, tmp);
            // get string from dex by stringId
            strObj = dvmResolveString(curMethod->clazz, tmp);
            assert(strObj != nullptr);
            SET_REGISTER(vdst, (u4) strObj);
        }
            FINISH(3);
            OP_END

        case OP_CONST_CLASS:
            HANDLE_OPCODE(OP_CONST_CLASS /*vAA, class@BBBB*/) {
            jclass clazz = nullptr;

            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOGD("|const-class v%d class@0x%04x", vdst, ref);
            // get clazz from dex by clazzId
            clazz = dvmResolveClass(curMethod->clazz, ref);
            if (clazz == nullptr) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER(vdst, (u4) clazz);
        }
            FINISH(2);
            OP_END

            // 0x1d-1e
        case OP_MONITOR_ENTER:
            HANDLE_OPCODE(OP_MONITOR_ENTER /*vAA*/) {
            jobject obj;

            vsrc1 = INST_AA(inst);
            LOGD("|monitor-enter v%d %s(0x%08x)",
                 vsrc1, kSpacing + 6, GET_REGISTER(vsrc1));
            obj = (jobject) GET_REGISTER(vsrc1);
            if (!checkForNull(obj)) {
                GOTO_exceptionThrown();
            }
            LOGD("+ locking %p %s", obj, getClassObjectByJObject(obj)->descriptor);
            (*env).MonitorEnter(obj);
        }
            FINISH(1);
            OP_END

        case OP_MONITOR_EXIT:
            HANDLE_OPCODE(OP_MONITOR_EXIT /*vAA*/) {
            jobject obj;

            vsrc1 = INST_AA(inst);
            LOGD("|monitor-exit v%d %s(0x%08x)",
                 vsrc1, kSpacing + 5, GET_REGISTER(vsrc1));
            obj = (jobject) GET_REGISTER(vsrc1);
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
            LOGD("+ unlocking %p %s", obj, getClassObjectByJObject(obj)->descriptor);
            if (!(*env).MonitorExit(obj)) {
                ADJUST_PC(1);
                GOTO_exceptionThrown();
            }
        }
            FINISH(1);
            OP_END

            // 0x1f-26
        case OP_CHECK_CAST:
            HANDLE_OPCODE(OP_CHECK_CAST /*vAA, class@BBBB*/) {
            jclass clazz = nullptr;
            jobject obj;

            vsrc1 = INST_AA(inst);
            ref = FETCH(1);         /* class to check against */
            LOGD("|check-cast v%d,class@0x%04x", vsrc1, ref);

            obj = (jobject) GET_REGISTER(vsrc1);
            if (obj != nullptr) {
                //  get clazz from dex by clazzId
                clazz = dvmResolveClass(curMethod->clazz, ref);
                if (clazz == nullptr) {
                    GOTO_exceptionThrown();
                }

                if (!(*env).IsInstanceOf(obj, clazz)) {
                    dvmThrowClassCastException((*env).GetObjectClass(obj), clazz);
                    GOTO_exceptionThrown();
                }
            }
        }
            FINISH(2);
            OP_END

        case OP_INSTANCE_OF:
            HANDLE_OPCODE(OP_INSTANCE_OF /*vA, vB, class@CCCC*/) {
            jclass clazz = nullptr;
            jobject obj;

            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);   /* object to check */
            ref = FETCH(1);         /* class to check against */
            LOGD("|instance-of v%d,v%d,class@0x%04x", vdst, vsrc1, ref);

            obj = (jobject) GET_REGISTER(vsrc1);
            if (obj == nullptr) {
                SET_REGISTER(vdst, 0);
            } else {
                // get clazz from dex by clazzId
                clazz = dvmResolveClass(curMethod->clazz, ref);
                if (clazz == nullptr) {
                    GOTO_exceptionThrown();
                }
            }
            // check type
            SET_REGISTER(vdst, static_cast<u4>((*env).IsInstanceOf(obj, clazz)));

        }
            FINISH(2);
            OP_END

        case OP_ARRAY_LENGTH:
            HANDLE_OPCODE(OP_ARRAY_LENGTH /*vA, vB*/) {
            jobjectArray arrayObj;

            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            arrayObj = (jobjectArray) GET_REGISTER(vsrc1);
            LOGD("|array-length v%d,v%d  (%p)", vdst, vsrc1, arrayObj);
            if (!checkForNull(arrayObj)) {
                GOTO_exceptionThrown();
            }
            /* verifier guarantees this is an array reference */
            u4 arrayLen = static_cast<u4>((*env).GetArrayLength(arrayObj));
            SET_REGISTER(vdst, arrayLen);
        }
            FINISH(1);
            OP_END

        case OP_NEW_INSTANCE:
            HANDLE_OPCODE(OP_NEW_INSTANCE /*vAA, class@BBBB*/) {
            jclass clazz = nullptr;
            jobject newObj = nullptr;

            vdst = INST_AA(inst);
            ref = FETCH(1);
            LOGD("|new-instance v%d,class@0x%04x", vdst, ref);
            //  get clazz from dex by clazzId
            clazz = dvmResolveClass(curMethod->clazz, ref);
            if (clazz == nullptr) {
                GOTO_exceptionThrown();
            }
            newObj = (*env).AllocObject(clazz);
            if (!checkForNull(newObj)) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER(vdst, (u4) newObj);
        }
            FINISH(2);
            OP_END

        case OP_NEW_ARRAY:
            HANDLE_OPCODE(OP_NEW_ARRAY /*vA, vB, class@CCCC*/) {
            jarray newArray;
            s4 length;

            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);       /* length reg */
            ref = FETCH(1);
            LOGD("|new-array v%d,v%d,class@0x%04x  (%d elements)",
                 vdst, vsrc1, ref, (s4) GET_REGISTER(vsrc1));
            length = (s4) GET_REGISTER(vsrc1);
            if (length < 0) {
                dvmThrowNegativeArraySizeException(length);
                GOTO_exceptionThrown()
            }
            newArray = dvmAllocArrayByClass(length, curMethod->clazz, ref);
            if (newArray == nullptr) {
                GOTO_exceptionThrown();
            }
            SET_REGISTER(vdst, (u4) newArray);
        }
            FINISH(2);
            OP_END

        case OP_FILLED_NEW_ARRAY:
            HANDLE_OPCODE(OP_FILLED_NEW_ARRAY /*vB, {vD, vE, vF, vG, vA}, class@CCCC*/)
            GOTO_invoke(filledNewArray, false);
            OP_END

        case OP_FILLED_NEW_ARRAY_RANGE:
            HANDLE_OPCODE(OP_FILLED_NEW_ARRAY_RANGE /*{vCCCC..v(CCCC+AA-1)}, class@BBBB*/)
            GOTO_invoke(filledNewArray, true);
            OP_END

        case OP_FILL_ARRAY_DATA:
            HANDLE_OPCODE(OP_FILL_ARRAY_DATA)   /*vAA, +BBBBBBBB*/
        {
            const u2 *arrayData;
            s4 offset;
            jarray arrayObj;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOGD("|fill-array-data v%d +0x%04x", vsrc1, offset);
            arrayData = pc + offset;       // offset in 16-bit units

            arrayObj = (jarray) GET_REGISTER(vsrc1);
            if (!dvmInterpHandleFillArrayData(arrayObj, arrayData)) {
                GOTO_exceptionThrown();
            }
            FINISH(3);
        }
            OP_END


        case OP_THROW:
            HANDLE_OPCODE(OP_THROW /*vAA*/) {
            jthrowable obj;

            vsrc1 = INST_AA(inst);
            LOGD("|throw v%d  (%p)", vsrc1, (void *) GET_REGISTER(vsrc1));
            obj = (jthrowable) GET_REGISTER(vsrc1);
            if (!checkForNull(obj)) {
                /* will throw a null pointer exception */
                LOGE("Bad exception");
                GOTO_exceptionThrown();
            } else {
                /* use the requested exception */
                (*env).Throw(obj);
                GOTO_exceptionThrown();
            }
        }
            OP_END

            // 0x28-0x3d
        case OP_GOTO:
            HANDLE_OPCODE(OP_GOTO /*+AA*/)
            vdst = INST_AA(inst);
            if ((s1) vdst < 0) {
                LOGD("|goto -0x%02x", -((s1) vdst));
            } else {
                LOGD("|goto +0x%02x", ((s1) vdst));
                LOGD("> branch taken");
            }

            if ((s1) vdst < 0) {PERIODIC_CHECKS((s1) vdst); }
            FINISH((s1) vdst);
            OP_END

        case OP_GOTO_16:
            HANDLE_OPCODE(OP_GOTO_16 /*+AAAA*/) {
            s4 offset = (s2) FETCH(1);          /* sign-extend next code unit */

            if (offset < 0)
                LOGD("|goto/16 -0x%04x", -offset);
            else
                LOGD("|goto/16 +0x%04x", offset);
            LOGD("> branch taken");

            if (offset < 0) PERIODIC_CHECKS(offset);
            FINISH(offset);
        }
            OP_END

        case OP_GOTO_32:
            HANDLE_OPCODE(OP_GOTO_32 /*+AAAAAAAA*/) {
            s4 offset = FETCH(1);               /* low-order 16 bits */
            offset |= ((s4) FETCH(2)) << 16;    /* high-order 16 bits */

            if (offset < 0)
                LOGD("|goto/32 -0x%08x", -offset);
            else
                LOGD("|goto/32 +0x%08x", offset);
            LOGD("> branch taken");

            /* allowed to branch to self */
            if (offset <= 0) PERIODIC_CHECKS(offset);
            FINISH(offset);
        }
            OP_END

        case OP_PACKED_SWITCH:
            HANDLE_OPCODE(OP_PACKED_SWITCH /*vAA, +BBBB*/) {
            const u2 *switchData;
            u4 testVal;
            s4 offset;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOGD("|packed-switch v%d +0x%04x", vsrc1, offset);
            switchData = pc + offset;       // offset in 16-bit units

            testVal = GET_REGISTER(vsrc1);

            offset = dvmInterpHandlePackedSwitch(switchData, testVal);
            LOGD("> branch taken (0x%04x)", offset);

            /* uncommon */
            if (offset <= 0) PERIODIC_CHECKS(offset);
            FINISH(offset);
        }
            OP_END

        case OP_SPARSE_SWITCH:
            HANDLE_OPCODE(OP_SPARSE_SWITCH /*vAA, +BBBB*/) {
            const u2 *switchData;
            u4 testVal;
            s4 offset;

            vsrc1 = INST_AA(inst);
            offset = FETCH(1) | (((s4) FETCH(2)) << 16);
            LOGD("|sparse-switch v%d +0x%04x", vsrc1, offset);
            switchData = pc + offset;       // offset in 16-bit units

            testVal = GET_REGISTER(vsrc1);

            offset = dvmInterpHandleSparseSwitch(switchData, testVal);
            LOGD("> branch taken (0x%04x)", offset);

            /* uncommon */
            if (offset <= 0) PERIODIC_CHECKS(offset);
            FINISH(offset);
        }
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

        case OP_IF_EQ:
        HANDLE_OP_IF_XX(OP_IF_EQ, "eq", ==)
            OP_END

        case OP_IF_NE:
        HANDLE_OP_IF_XX(OP_IF_NE, "ne", !=)
            OP_END

        case OP_IF_LT:
        HANDLE_OP_IF_XX(OP_IF_LT, "lt", <)
            OP_END

        case OP_IF_GE:
        HANDLE_OP_IF_XX(OP_IF_GE, "ge", >=)
            OP_END

        case OP_IF_GT:
        HANDLE_OP_IF_XX(OP_IF_GT, "gt", >)
            OP_END

        case OP_IF_LE:
        HANDLE_OP_IF_XX(OP_IF_LE, "le", <=)
            OP_END

        case OP_IF_EQZ:
        HANDLE_OP_IF_XXZ(OP_IF_EQZ, "eqz", ==)
            OP_END

        case OP_IF_NEZ:
        HANDLE_OP_IF_XXZ(OP_IF_NEZ, "nez", !=)
            OP_END

        case OP_IF_LTZ:
        HANDLE_OP_IF_XXZ(OP_IF_LTZ, "ltz", <)
            OP_END

        case OP_IF_GEZ:
        HANDLE_OP_IF_XXZ(OP_IF_GEZ, "gez", >=)
            OP_END

        case OP_IF_GTZ:
        HANDLE_OP_IF_XXZ(OP_IF_GTZ, "gtz", >)
            OP_END

        case OP_IF_LEZ:
        HANDLE_OP_IF_XXZ(OP_IF_LEZ, "lez", <=)
            OP_END

            // 0x44-0x6d
        case OP_AGET: HANDLE_OP_AGET(OP_AGET, "-normal", Int, jint,)
            OP_END

        case OP_AGET_WIDE: HANDLE_OP_AGET(OP_AGET_WIDE, "-wide", Long, jlong, _WIDE)
            OP_END

        case OP_AGET_OBJECT:
            HANDLE_OPCODE(OP_AGET_OBJECT /*vAA, vBB, vCC*/) {
            jobjectArray arrayObj;
            u2 arrayInfo;
            vdst = INST_AA(inst);
            arrayInfo = FETCH(1);
            vsrc1 = static_cast<u2>(arrayInfo & 0xff);    /* array ptr */
            vsrc2 = arrayInfo >> 8;      /* index */
            LOGD("|aget%s v%d,v%d,v%d", "-object", vdst, vsrc1, vsrc2);
            arrayObj = (jobjectArray) GET_REGISTER(vsrc1);
            if (!checkForNull(arrayObj)) {
                GOTO_exceptionThrown();
            }
            if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {
                dvmThrowArrayIndexOutOfBoundsException(
                        (*env).GetArrayLength(arrayObj), GET_REGISTER(vsrc2));
                GOTO_exceptionThrown();
            }
            SET_REGISTER(vdst, (u4) (*env).GetObjectArrayElement(arrayObj, GET_REGISTER(vsrc2)));
            LOGD("+ AGET[%d]=%#x", GET_REGISTER(vsrc2), GET_REGISTER(vdst));
        }
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

        case OP_APUT_OBJECT:
            HANDLE_OPCODE(OP_APUT_OBJECT /*vAA, vBB, vCC*/) {
            jobjectArray arrayObj;
            jobject obj;
            u2 arrayInfo;
            vdst = INST_AA(inst);       /* AA: source value */
            arrayInfo = FETCH(1);
            vsrc1 = static_cast<u2>(arrayInfo & 0xff);   /* BB: array ptr */
            vsrc2 = arrayInfo >> 8;     /* CC: index */
            LOGD("|aput%s v%d,v%d,v%d", "-object", vdst, vsrc1, vsrc2);
            arrayObj = (jobjectArray) GET_REGISTER(vsrc1);
            if (!checkForNull((jobject) arrayObj)) {
                GOTO_exceptionThrown();
            }
            if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {
                dvmThrowArrayIndexOutOfBoundsException(
                        (*env).GetArrayLength(arrayObj), GET_REGISTER(vsrc2));
                GOTO_exceptionThrown();
            }
            obj = (jobject) GET_REGISTER(vdst);

            if (!checkForNull(obj)) {
                GOTO_exceptionThrown();
            }
            if (!dvmCanPutArrayElement(obj, arrayObj)) {
                LOGE("Can't put a '%s'(%p) into array type='%s'(%p)",
                     getClassObjectByJObject(obj)->descriptor, obj,
                     getClassObjectByJObject(arrayObj)->descriptor, arrayObj);
                dvmThrowArrayStoreExceptionIncompatibleElement(obj, arrayObj);
                GOTO_exceptionThrown();
            }

            LOGD("+ APUT[%d]=0x%08x", GET_REGISTER(vsrc2), GET_REGISTER(vdst));
            (*env).SetObjectArrayElement(arrayObj, GET_REGISTER(vsrc2), obj);
        }
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

        case OP_IGET_OBJECT: HANDLE_IGET_X(OP_IGET_OBJECT, "-object", s4, _AS_OBJECT)
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


            /*
             * The VM spec says we should verify that the reference being stored into
             * the field is assignment compatible.  In practice, many popular VMs don't
             * do this because it slows down a very common operation.  It's not so bad
             * for us, since "dexopt" quickens it whenever possible, but it's still an
             * issue.
             *
             * To make this spec-complaint, we'd need to add a ClassObject pointer to
             * the Field struct, resolve the field's type descriptor at link or class
             * init time, and then verify the type here.
             */
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

        case OP_SGET_OBJECT: HANDLE_SGET_X(OP_SGET_OBJECT, "-object", s4, _AS_OBJECT)
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
        case OP_INVOKE_VIRTUAL:
            HANDLE_OPCODE(OP_INVOKE_VIRTUAL /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeVirtual, false);
            OP_END

        case OP_INVOKE_SUPER:
            HANDLE_OPCODE(OP_INVOKE_SUPER /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeSuper, false);
            OP_END

        case OP_INVOKE_DIRECT:
            HANDLE_OPCODE(OP_INVOKE_DIRECT /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeDirect, false);
            OP_END

        case OP_INVOKE_STATIC:
            HANDLE_OPCODE(OP_INVOKE_STATIC /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeStatic, false);
            OP_END

        case OP_INVOKE_INTERFACE:
            HANDLE_OPCODE(OP_INVOKE_INTERFACE /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
            GOTO_invoke(invokeInterface, false);
            OP_END


        case OP_INVOKE_VIRTUAL_RANGE:
            HANDLE_OPCODE(OP_INVOKE_VIRTUAL_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeVirtual, true);
            OP_END

        case OP_INVOKE_SUPER_RANGE:
            HANDLE_OPCODE(OP_INVOKE_SUPER_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeSuper, true);
            OP_END

        case OP_INVOKE_DIRECT_RANGE:
            HANDLE_OPCODE(OP_INVOKE_DIRECT_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeDirect, true);
            OP_END

        case OP_INVOKE_STATIC_RANGE:
            HANDLE_OPCODE(OP_INVOKE_STATIC_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeStatic, true);
            OP_END

        case OP_INVOKE_INTERFACE_RANGE:
            HANDLE_OPCODE(OP_INVOKE_INTERFACE_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
            GOTO_invoke(invokeInterface, true);
            OP_END

            // 0x7b-0x80
        case OP_NEG_INT:
        HANDLE_UNOP(OP_NEG_INT, "neg-int", -, ,)
            OP_END

        case OP_NOT_INT:
        HANDLE_UNOP(OP_NOT_INT, "not-int", , ^
                0xffffffff,)
            OP_END

        case OP_NEG_LONG:
        HANDLE_UNOP(OP_NEG_LONG, "neg-long", -, , _WIDE)
            OP_END

        case OP_NOT_LONG:
        HANDLE_UNOP(OP_NOT_LONG, "not-long", , ^
                0xffffffffffffffffULL, _WIDE)
            OP_END

        case OP_NEG_FLOAT:
        HANDLE_UNOP(OP_NEG_FLOAT, "neg-float", -, , _FLOAT)
            OP_END

        case OP_NEG_DOUBLE:
        HANDLE_UNOP(OP_NEG_DOUBLE, "neg-double", -, , _DOUBLE)
            OP_END

            // 0x84-0x8f
        case OP_INT_TO_LONG:
        HANDLE_NUMCONV(OP_INT_TO_LONG, "int-to-long", _INT, _WIDE)
            OP_END

        case OP_INT_TO_FLOAT:
        HANDLE_NUMCONV(OP_INT_TO_FLOAT, "int-to-float", _INT, _FLOAT)
            OP_END

        case OP_INT_TO_DOUBLE:
        HANDLE_NUMCONV(OP_INT_TO_DOUBLE, "int-to-double", _INT, _DOUBLE)
            OP_END

        case OP_LONG_TO_INT:
        HANDLE_NUMCONV(OP_LONG_TO_INT, "long-to-int", _WIDE, _INT)
            OP_END

        case OP_LONG_TO_FLOAT:
        HANDLE_NUMCONV(OP_LONG_TO_FLOAT, "long-to-float", _WIDE, _FLOAT)
            OP_END

        case OP_LONG_TO_DOUBLE:
        HANDLE_NUMCONV(OP_LONG_TO_DOUBLE, "long-to-double", _WIDE, _DOUBLE)
            OP_END

        case OP_FLOAT_TO_INT: HANDLE_FLOAT_TO_INT(OP_FLOAT_TO_INT, "float-to-int",
                                                  float, _FLOAT, s4, _INT)
            OP_END

        case OP_FLOAT_TO_LONG: HANDLE_FLOAT_TO_INT(OP_FLOAT_TO_LONG, "float-to-long",
                                                   float, _FLOAT, s8, _WIDE)
            OP_END

        case OP_FLOAT_TO_DOUBLE:
        HANDLE_NUMCONV(OP_FLOAT_TO_DOUBLE, "float-to-double", _FLOAT, _DOUBLE)
            OP_END

        case OP_DOUBLE_TO_INT: HANDLE_FLOAT_TO_INT(OP_DOUBLE_TO_INT, "double-to-int",
                                                   double, _DOUBLE, s4, _INT)
            OP_END

        case OP_DOUBLE_TO_LONG: HANDLE_FLOAT_TO_INT(OP_DOUBLE_TO_LONG, "double-to-long",
                                                    double, _DOUBLE, s8, _WIDE)
            OP_END

        case OP_DOUBLE_TO_FLOAT:
        HANDLE_NUMCONV(OP_DOUBLE_TO_FLOAT, "double-to-float", _DOUBLE, _FLOAT)
            OP_END

        case OP_INT_TO_BYTE:
        HANDLE_INT_TO_SMALL(OP_INT_TO_BYTE, "byte", s1)
            OP_END

        case OP_INT_TO_CHAR:
        HANDLE_INT_TO_SMALL(OP_INT_TO_CHAR, "char", u2)
            OP_END

        case OP_INT_TO_SHORT:
        HANDLE_INT_TO_SMALL(OP_INT_TO_SHORT, "short", s2)    /* want sign bit */
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

        case OP_REM_FLOAT:
            HANDLE_OPCODE(OP_REM_FLOAT /*vAA, vBB, vCC*/) {
            u2 srcRegs;
            vdst = INST_AA(inst);
            srcRegs = FETCH(1);
            vsrc1 = static_cast<u2>(srcRegs & 0xff);
            vsrc2 = srcRegs >> 8;
            LOGD("|%s-float v%d,v%d,v%d", "mod", vdst, vsrc1, vsrc2);
            SET_REGISTER_FLOAT(vdst, fmodf(GET_REGISTER_FLOAT(vsrc1), GET_REGISTER_FLOAT(vsrc2)));
        }
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

        case OP_REM_DOUBLE:
            HANDLE_OPCODE(OP_REM_DOUBLE /*vAA, vBB, vCC*/) {
            u2 srcRegs;
            vdst = INST_AA(inst);
            srcRegs = FETCH(1);
            vsrc1 = static_cast<u2>(srcRegs & 0xff);
            vsrc2 = srcRegs >> 8;
            LOGD("|%s-double v%d,v%d,v%d", "mod", vdst, vsrc1, vsrc2);
            SET_REGISTER_DOUBLE(vdst, fmod(GET_REGISTER_DOUBLE(vsrc1), GET_REGISTER_DOUBLE(vsrc2)));
        }
            FINISH(2);
            OP_END

            // 0xb0-0xba
        case OP_ADD_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_ADD_INT_2ADDR, "add", +, 0)
            OP_END

        case OP_SUB_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_SUB_INT_2ADDR, "sub", -, 0)
            OP_END

        case OP_MUL_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_MUL_INT_2ADDR, "mul", *, 0)
            OP_END

        case OP_DIV_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_DIV_INT_2ADDR, "div", /, 1)
            OP_END

        case OP_REM_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_REM_INT_2ADDR, "rem", %, 2)
            OP_END

        case OP_AND_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_AND_INT_2ADDR, "and", &, 0)
            OP_END

        case OP_OR_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_OR_INT_2ADDR, "or", |, 0)
            OP_END

        case OP_XOR_INT_2ADDR:
        HANDLE_OP_X_INT_2ADDR(OP_XOR_INT_2ADDR, "xor", ^, 0)
            OP_END

        case OP_SHL_INT_2ADDR:
        HANDLE_OP_SHX_INT_2ADDR(OP_SHL_INT_2ADDR, "shl", (s4), <<)
            OP_END

        case OP_SHR_INT_2ADDR:
        HANDLE_OP_SHX_INT_2ADDR(OP_SHR_INT_2ADDR, "shr", (s4), >>)
            OP_END

        case OP_USHR_INT_2ADDR:
        HANDLE_OP_SHX_INT_2ADDR(OP_USHR_INT_2ADDR, "ushr", (u4), >>)
            OP_END

            //0xbb-0xc5
        case OP_ADD_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_ADD_LONG_2ADDR, "add", +, 0)
            OP_END

        case OP_SUB_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_SUB_LONG_2ADDR, "sub", -, 0)
            OP_END

        case OP_MUL_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_MUL_LONG_2ADDR, "mul", *, 0)
            OP_END

        case OP_DIV_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_DIV_LONG_2ADDR, "div", /, 1)
            OP_END

        case OP_REM_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_REM_LONG_2ADDR, "rem", %, 2)
            OP_END

        case OP_AND_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_AND_LONG_2ADDR, "and", &, 0)
            OP_END

        case OP_OR_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_OR_LONG_2ADDR, "or", |, 0)
            OP_END

        case OP_XOR_LONG_2ADDR:
        HANDLE_OP_X_LONG_2ADDR(OP_XOR_LONG_2ADDR, "xor", ^, 0)
            OP_END

        case OP_SHL_LONG_2ADDR:
        HANDLE_OP_SHX_LONG_2ADDR(OP_SHL_LONG_2ADDR, "shl", (s8), <<)
            OP_END

        case OP_SHR_LONG_2ADDR:
        HANDLE_OP_SHX_LONG_2ADDR(OP_SHR_LONG_2ADDR, "shr", (s8), >>)
            OP_END

        case OP_USHR_LONG_2ADDR:
        HANDLE_OP_SHX_LONG_2ADDR(OP_USHR_LONG_2ADDR, "ushr", (u8), >>)
            OP_END

            // 0xc6-0xca
        case OP_ADD_FLOAT_2ADDR:
        HANDLE_OP_X_FLOAT_2ADDR(OP_ADD_FLOAT_2ADDR, "add", +)
            OP_END

        case OP_SUB_FLOAT_2ADDR:
        HANDLE_OP_X_FLOAT_2ADDR(OP_SUB_FLOAT_2ADDR, "sub", -)
            OP_END

        case OP_MUL_FLOAT_2ADDR:
        HANDLE_OP_X_FLOAT_2ADDR(OP_MUL_FLOAT_2ADDR, "mul", *)
            OP_END

        case OP_DIV_FLOAT_2ADDR:
        HANDLE_OP_X_FLOAT_2ADDR(OP_DIV_FLOAT_2ADDR, "div", /)
            OP_END

        case OP_REM_FLOAT_2ADDR:
            HANDLE_OPCODE(OP_REM_FLOAT_2ADDR /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOGD("|%s-float-2addr v%d,v%d", "mod", vdst, vsrc1);
            SET_REGISTER_FLOAT(vdst, fmodf(GET_REGISTER_FLOAT(vdst), GET_REGISTER_FLOAT(vsrc1)));
            FINISH(1);
            OP_END

            // 0xcb-0xcf
        case OP_ADD_DOUBLE_2ADDR:
        HANDLE_OP_X_DOUBLE_2ADDR(OP_ADD_DOUBLE_2ADDR, "add", +)
            OP_END

        case OP_SUB_DOUBLE_2ADDR:
        HANDLE_OP_X_DOUBLE_2ADDR(OP_SUB_DOUBLE_2ADDR, "sub", -)
            OP_END

        case OP_MUL_DOUBLE_2ADDR:
        HANDLE_OP_X_DOUBLE_2ADDR(OP_MUL_DOUBLE_2ADDR, "mul", *)
            OP_END

        case OP_DIV_DOUBLE_2ADDR:
        HANDLE_OP_X_DOUBLE_2ADDR(OP_DIV_DOUBLE_2ADDR, "div", /)
            OP_END

        case OP_REM_DOUBLE_2ADDR:
            HANDLE_OPCODE(OP_REM_DOUBLE_2ADDR /*vA, vB*/)
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            LOGD("|%s-double-2addr v%d,v%d", "mod", vdst, vsrc1);
            SET_REGISTER_DOUBLE(vdst, fmod(GET_REGISTER_DOUBLE(vdst), GET_REGISTER_DOUBLE(vsrc1)));
            FINISH(1);
            OP_END

            // 0xd0-0xd7
        case OP_ADD_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_ADD_INT_LIT16, "add", +, 0)
            OP_END

        case OP_RSUB_INT:
            HANDLE_OPCODE(OP_RSUB_INT /*vA, vB, #+CCCC*/) {
            vdst = static_cast<u2>(INST_A(inst));
            vsrc1 = INST_B(inst);
            vsrc2 = FETCH(1);
            LOGD("|rsub-int v%d,v%d,#+0x%04x", vdst, vsrc1, vsrc2);
            SET_REGISTER(vdst, (s2) vsrc2 - (s4) GET_REGISTER(vsrc1));
        }
            FINISH(2);
            OP_END

        case OP_MUL_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_MUL_INT_LIT16, "mul", *, 0)
            OP_END

        case OP_DIV_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_DIV_INT_LIT16, "div", /, 1)
            OP_END

        case OP_REM_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_REM_INT_LIT16, "rem", %, 2)
            OP_END

        case OP_AND_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_AND_INT_LIT16, "and", &, 0)
            OP_END

        case OP_OR_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_OR_INT_LIT16, "or", |, 0)
            OP_END

        case OP_XOR_INT_LIT16:
        HANDLE_OP_X_INT_LIT16(OP_XOR_INT_LIT16, "xor", ^, 0)
            OP_END

            // 0xd8-0xe2
        case OP_ADD_INT_LIT8: HANDLE_OP_X_INT_LIT8(OP_ADD_INT_LIT8, "add", +, 0)
            OP_END

        case OP_RSUB_INT_LIT8:
            HANDLE_OPCODE(OP_RSUB_INT_LIT8 /*vAA, vBB, #+CC*/) {
            u2 litInfo;
            vdst = INST_AA(inst);
            litInfo = FETCH(1);
            vsrc1 = static_cast<u2>(litInfo & 0xff);
            vsrc2 = litInfo >> 8;
            LOGD("|%s-int/lit8 v%d,v%d,#+0x%02x", "rsub", vdst, vsrc1, vsrc2);
            SET_REGISTER(vdst, (s1) vsrc2 - (s4) GET_REGISTER(vsrc1));
        }
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

        case OP_IGET_OBJECT_VOLATILE: HANDLE_IGET_X(OP_IGET_OBJECT_VOLATILE, "-object-volatile", s4,
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

        case OP_THROW_VERIFICATION_ERROR:
            HANDLE_OPCODE(OP_THROW_VERIFICATION_ERROR)
            vsrc1 = INST_AA(inst);
            ref = FETCH(1);             /* class/field/method ref */
            dvmThrowVerificationError(curMethod, vsrc1, ref);
            GOTO_exceptionThrown();
            OP_END

            //0xf2-0xfe
//        case OP_IGET_QUICK:
//            HANDLE_IGET_X_QUICK(OP_IGET_QUICK,          "", Int, )
//            OP_END
//
//        case OP_IGET_WIDE_QUICK:
//            HANDLE_IGET_X_QUICK(OP_IGET_WIDE_QUICK,     "-wide", Long, _WIDE)
//            OP_END
//
//        case OP_IGET_OBJECT_QUICK:
//            HANDLE_IGET_X_QUICK(OP_IGET_OBJECT_QUICK,   "-object", Object, _AS_OBJECT)
//            OP_END
//
//        case OP_IPUT_QUICK:
//            HANDLE_IPUT_X_QUICK(OP_IPUT_QUICK,          "", Int, )
//            OP_END
//
//        case OP_IPUT_WIDE_QUICK:
//            HANDLE_IPUT_X_QUICK(OP_IPUT_WIDE_QUICK,     "-wide", Long, _WIDE)
//            OP_END
//
//        case OP_IPUT_OBJECT_QUICK:
//            HANDLE_IPUT_X_QUICK(OP_IPUT_OBJECT_QUICK,   "-object", Object, _AS_OBJECT)
//            OP_END
//
//        case OP_INVOKE_VIRTUAL_QUICK:
//            HANDLE_OPCODE(OP_INVOKE_VIRTUAL_QUICK /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
//            GOTO_invoke(invokeVirtualQuick, false);
//            OP_END
//
//        case OP_INVOKE_VIRTUAL_QUICK_RANGE:
//            HANDLE_OPCODE(OP_INVOKE_VIRTUAL_QUICK_RANGE/*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
//            GOTO_invoke(invokeVirtualQuick, true);
//            OP_END
//
//        case OP_INVOKE_SUPER_QUICK:
//            HANDLE_OPCODE(OP_INVOKE_SUPER_QUICK /*vB, {vD, vE, vF, vG, vA}, meth@CCCC*/)
//            GOTO_invoke(invokeSuperQuick, false);
//            OP_END
//
//        case OP_INVOKE_SUPER_QUICK_RANGE:
//            HANDLE_OPCODE(OP_INVOKE_SUPER_QUICK_RANGE /*{vCCCC..v(CCCC+AA-1)}, meth@BBBB*/)
//            GOTO_invoke(invokeSuperQuick, true);
//            OP_END

        case OP_IPUT_OBJECT_VOLATILE: HANDLE_IPUT_X(OP_IPUT_OBJECT_VOLATILE, "-object-volatile",
                                                    _AS_OBJECT)
            OP_END

        case OP_SGET_OBJECT_VOLATILE: HANDLE_SGET_X(OP_SGET_OBJECT_VOLATILE, "-object-volatile", s4,
                                                    _AS_OBJECT)
            OP_END

        case OP_SPUT_OBJECT_VOLATILE: HANDLE_SPUT_X(OP_SPUT_OBJECT_VOLATILE, "-object-volatile",
                                                    _AS_OBJECT)
            OP_END
        default:
            // TODO: error code, throw exception
            LOGE("unknown opcode 0x%02x\n", curOp);
            dvmThrowRuntimeException("unknown opcode");
            return;
    }


    bail:
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
        u4 *contents;
        char typeCh;
        u4 arg5;

        ref = FETCH(1);             /* class ref */
        vdst = FETCH(2);            /* first 4 regs -or- range base */

        if (methodCallRange) {
            vsrc1 = INST_AA(inst);  /* #of elements */
            arg5 = -1;              /* silence compiler warning */
            LOGD("|filled-new-array-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            arg5 = static_cast<u4>(INST_A(inst));
            vsrc1 = INST_B(inst);   /* #of elements */
            LOGD("|filled-new-array args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1, ref, vdst, arg5);
        }

        /*
        * Resolve the array class.
        */
        arrayClass = dvmResolveClass(curMethod->clazz, ref);
        if (arrayClass == nullptr) {
            GOTO_bail();
        }

        ClassObject *pClassObject = getClassObjectByJClass(arrayClass);
        /* verifier guarantees this is an array class */
        assert(pClassObject->descriptor[0] == '[');

        /*
         * Create an array of the specified type.
         */
        LOGD("+++ filled-new-array type is '%s'", pClassObject->descriptor);
        typeCh = pClassObject->descriptor[1];
        if (typeCh == 'D' || typeCh == 'J') {
            /* category 2 primitives not allowed */
            dvmThrowRuntimeException("bad filled array req");
            GOTO_bail();
        } else if (typeCh != 'L' && typeCh != '[' && typeCh != 'I') {
            LOGE("non-int primitives not implemented");
            dvmThrowInternalError(
                    "filled-new-array not implemented for anything but 'int'");
            GOTO_bail();
        }

        contents = new u4[vsrc1];
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

        if (typeCh == 'L' || typeCh == '[') {
            string tmp = pClassObject->elementClass->descriptor;
            if (tmp[0] == 'L') {
                tmp = tmp.substr(1, tmp.size() - 1);
            }
            jclass elementClazz = (*env).FindClass(tmp.data());
            newArray = (*env).NewObjectArray(vsrc1, elementClazz, nullptr);
            for (int i = 0; i < vsrc1; i++) {
                (*env).SetObjectArrayElement(reinterpret_cast<jobjectArray>(newArray), i,
                                             reinterpret_cast<jobject>(contents[i]));
            }
        } else {
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
        u2 thisReg;

        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOGD("|invoke-super-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
            thisReg = vdst;
        } else {
            LOGD("|invoke-super args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
            thisReg = static_cast<u2>(vdst & 0x0f);
        }

        /* impossible in well-formed code, but we must check nevertheless */
        if (!checkForNull((jobject) GET_REGISTER(thisReg))) {
            GOTO_bail();
        }

        methodToCallType = METHOD_SUPER;
        methodToCall = dvmResolveMethod(curMethod->clazz, ref, methodToCallType,
                                        &methodToCallClazz);
        if (methodToCall == nullptr) {
            GOTO_bail();
        }
        assert(!dvmIsAbstractMethod(reinterpret_cast<Method *>(methodToCall)) ||
               (reinterpret_cast<Method *>(methodToCall))->nativeFunc != nullptr);

//        methodToCallClazz = (*env).GetSuperclass(methodToCallClazz);
        LOGD("+++ super-virtual[%d]=%s.%s",
             (reinterpret_cast<Method *>(methodToCall))->methodIndex,
             (reinterpret_cast<Method *>(methodToCall))->clazz->descriptor,
             (reinterpret_cast<Method *>(methodToCall))->name);

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END

//
//    GOTO_TARGET(invokeSuperQuick, bool methodCallRange)
//    {
//        u2 thisReg;
//
//        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
//        ref = FETCH(1);             /* vtable index */
//        vdst = FETCH(2);            /* 4 regs -or- first reg */
//
//        if (methodCallRange) {
//            LOGD("|invoke-super-quick-range args=%d @0x%04x {regs=v%d-v%d}",
//                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
//            thisReg = vdst;
//        } else {
//            LOGD("|invoke-super-quick args=%d @0x%04x {regs=0x%04x %x}",
//                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
//            thisReg = static_cast<u2>(vdst & 0x0f);
//        }
//        /* impossible in well-formed code, but we must check nevertheless */
//        if (!checkForNull((jobject) GET_REGISTER(thisReg))) {
//            GOTO_bail();
//        }
//
//        assert(ref < (unsigned int) curMethod->clazz->super->vtableCount);
//
//        /*
//         * Combine the object we found with the vtable offset in the
//         * method's class.
//         *
//         * We're using the current method's class' superclass, not the
//         * superclass of "this".  This is because we might be executing
//         * in a method inherited from a superclass, and we want to run
//         * in the method's class' superclass.
//         */
//        Method *pMethod = curMethod->clazz->super->vtable[ref];
//        if (pMethod == nullptr) {
//            LOGE("+ unknown method or access denied");
//            GOTO_bail();
//        }
//        methodToCall = reinterpret_cast<jmethodID>(pMethod);
//        assert(!dvmIsAbstractMethod(pMethod) || pMethod->nativeFunc != nullptr);
//
//        methodToCallType = METHOD_SUPER;
//
//        DexFile *pDexFile = curMethod->clazz->pDvmDex->pDexFile;
//        const DexProtoId *pProtoId = dexGetProtoId(pDexFile, pMethod->prototype.protoIdx);
//        const DexTypeId *pTypeId = dexGetTypeId(pDexFile, pProtoId->returnTypeIdx);
//        const DexStringId *pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
//        methodToCallReturn = dexGetStringData(pDexFile, pDexStringId);
//
//        string tmp = curMethod->clazz->super->descriptor;
//        assert(tmp.size() > 0 && tmp[0] == 'L');
//        tmp = tmp.substr(1, tmp.size() - 1);
//        methodToCallClazz = (*env).FindClass(tmp.data());
//
//        LOGD("+++ super-virtual[%d]=%s.%s", ref, pMethod->clazz->descriptor, pMethod->name);
//        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
//    }
//    GOTO_TARGET_END


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
            LOGD("|invoke-virtual-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);

        } else {
            assert((vsrc1 >> 4) > 0);
            LOGD("|invoke-virtual args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        /*
         * Resolve the method.  This is the correct method for the static
         * type of the object.  We also verify access permissions here.
         */
        methodToCallType = METHOD_VIRTUAL;
        methodToCall = dvmResolveMethod(curMethod->clazz, ref, methodToCallType,
                                        &methodToCallClazz);
        if (methodToCall == nullptr) {
            LOGE("+ unknown method or access denied");
            GOTO_bail();
        }

        assert(!dvmIsAbstractMethod(reinterpret_cast<Method *>(methodToCall)) ||
               (reinterpret_cast<Method *>(methodToCall))->nativeFunc != nullptr);
        assert(methodToCall != nullptr);
        LOGD("+++ virtual[%d]=%s.%s",
             (reinterpret_cast<Method *>(methodToCall))->methodIndex,
             (reinterpret_cast<Method *>(methodToCall))->clazz->descriptor,
             (reinterpret_cast<Method *>(methodToCall))->name);

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


//    GOTO_TARGET(invokeVirtualQuick, bool methodCallRange)
//    {
//        jobject thisPtr;
//        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
//        ref = FETCH(1);             /* vtable index */
//        vdst = FETCH(2);            /* 4 regs -or- first reg */
//
//        /*
//         * The object against which we are executing a method is always
//         * in the first argument.
//         */
//        if (methodCallRange) {
//            assert(vsrc1 > 0);
//            LOGD("|invoke-virtual-quick-range args=%d @0x%04x {regs=v%d-v%d}",
//                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
//            thisPtr = (jobject) GET_REGISTER(vdst);
//        } else {
//            assert((vsrc1 >> 4) > 0);
//            LOGD("|invoke-virtual-quick args=%d @0x%04x {regs=0x%04x %x}",
//                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
//            thisPtr = (jobject) GET_REGISTER(vdst & 0x0f);
//        }
//
//        /*
//         * Combine the object we found with the vtable offset in the
//         * method.
//         */
//        ClassObject *pClassObject = getClassObjectByJObject(thisPtr);
//        assert(ref < (unsigned int) pClassObject->vtableCount);
//        Method *pMethod = pClassObject->vtable[ref];
//        if (pMethod == nullptr) {
//            LOGE("+ unknown method or access denied");
//            GOTO_bail();
//        }
//        methodToCall = reinterpret_cast<jmethodID>(pMethod);
//
//        methodToCallClazz = (*env).GetObjectClass(thisPtr);
//
//        DexFile *pDexFile = pClassObject->pDvmDex->pDexFile;
//        const DexProtoId *pProtoId = dexGetProtoId(pDexFile, pMethod->prototype.protoIdx);
//        const DexTypeId *pTypeId = dexGetTypeId(pDexFile, pProtoId->returnTypeIdx);
//        const DexStringId *pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
//        methodToCallReturn = dexGetStringData(pDexFile, pDexStringId);
//
//        assert(!dvmIsAbstractMethod(pMethod) || pMethod->nativeFunc != nullptr);
//        LOGD("+++ virtual quick[%d]=%s.%s", pMethod->methodIndex, pMethod->clazz->descriptor,
//             pMethod->name);
//
//        methodToCallType = METHOD_VIRTUAL;
//        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
//    }
//    GOTO_TARGET_END



    GOTO_TARGET(invokeDirect, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOGD("|invoke-direct-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            LOGD("|invoke-direct args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        methodToCallType = METHOD_DIRECT;
        methodToCall = dvmResolveMethod(curMethod->clazz, ref, methodToCallType,
                                        &methodToCallClazz);

        if (methodToCall == nullptr) {
            LOGE("+ unknown direct method");     // should be impossible
            GOTO_bail();
        }

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END



    GOTO_TARGET(invokeStatic, bool methodCallRange)
    {
        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        if (methodCallRange) {
            LOGD("|invoke-static-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
        } else {
            LOGD("|invoke-static args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
        }

        methodToCallType = METHOD_STATIC;
        methodToCall = dvmResolveMethod(curMethod->clazz, ref, methodToCallType,
                                        &methodToCallClazz);
        if (methodToCall == nullptr) {
            LOGE("+ unknown method");
            GOTO_bail();
        }
        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END


    GOTO_TARGET(invokeInterface, bool methodCallRange)
    {
        jobject thisPtr;

        vsrc1 = INST_AA(inst);      /* AA (count) or BA (count + arg 5) */
        ref = FETCH(1);             /* method ref */
        vdst = FETCH(2);            /* 4 regs -or- first reg */

        /*
         * The object against which we are executing a method is always
         * in the first argument.
         */
        if (methodCallRange) {
            assert(vsrc1 > 0);
            LOGD("|invoke-interface-range args=%d @0x%04x {regs=v%d-v%d}",
                 vsrc1, ref, vdst, vdst + vsrc1 - 1);
            thisPtr = (jobject) GET_REGISTER(vdst);
        } else {
            assert((vsrc1 >> 4) > 0);
            LOGD("|invoke-interface args=%d @0x%04x {regs=0x%04x %x}",
                 vsrc1 >> 4, ref, vdst, vsrc1 & 0x0f);
            thisPtr = (jobject) GET_REGISTER(vdst & 0x0f);
        }

        if (!checkForNull(thisPtr)) {
            GOTO_bail();
        }

        /*
         * Given a class and a method index, find the Method* with the
         * actual code we want to execute.
         */
        methodToCallType = METHOD_INTERFACE;
        methodToCall = dvmResolveMethod(curMethod->clazz, ref, methodToCallType,
                                        &methodToCallClazz);
        if (methodToCall == nullptr) {
            /* impossible in verified DEX, need to check for it in unverified */
            dvmThrowIncompatibleClassChangeError("interface not implemented");
            GOTO_bail();
        }
        assert(!dvmIsAbstractMethod(reinterpret_cast<Method *>(methodToCall)) ||
               (reinterpret_cast<Method *>(methodToCall))->nativeFunc != nullptr);
        LOGD("+++ interface concrete=%s.%s",
             (reinterpret_cast<Method *>(methodToCall))->clazz->descriptor,
             (reinterpret_cast<Method *>(methodToCall))->name);

        GOTO_invokeMethod(methodCallRange, methodToCall, vsrc1, vdst);
    }
    GOTO_TARGET_END

    /*
     * General handling for invoke-{virtual,super,direct,static,interface},
     * including "quick" variants.
     *
     * Set "methodToCall" to the Method we're calling, and "methodCallRange"
     * depending on whether this is a "/range" instruction.
     *
     * For a range call:
     *  "vsrc1" holds the argument count (8 bits)
     *  "vdst" holds the first argument in the range
     * For a non-range call:
     *  "vsrc1" holds the argument count (4 bits) and the 5th argument index
     *  "vdst" holds four 4-bit register indices
     *
     * The caller must EXPORT_PC before jumping here, because any method
     * call can throw a stack overflow exception.
     */
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

        const char *methodToCallShorty = ((Method *) methodToCall)->shorty;

        /*
         * Copy args.  This may corrupt vsrc1/vdst.
         */
        int var_i = 0;
        int var_i_reg = 0;
        if (methodCallRange) {
            vars = new jvalue[vsrc1];
            if (methodToCallType != METHOD_STATIC) {
                thisPtr = (jobject) GET_REGISTER(vdst);
                var_i_reg = 1;
            }
            for (; var_i_reg < vsrc1; var_i_reg++, var_i++) {
                switch (methodToCallShorty[var_i + 1]) {
                    case 'D':
                        vars[var_i].d = GET_REGISTER_DOUBLE(vdst + var_i_reg);
                        var_i_reg++;
                        break;

                    case 'J':
                        vars[var_i].j = GET_REGISTER_WIDE(vdst + var_i_reg);
                        var_i_reg++;
                        break;

                    case '[':
                        while (methodToCallShorty[var_i + 1]) {
                            var_i++;
                        }
                        // not break;
                    default:
                        memcpy(vars + var_i, &(GET_REGISTER(vdst + var_i_reg)), sizeof(u4));
                        break;
                }
            }
        } else {
            u4 count = vsrc1 >> 4;

            assert(count <= 5);
            vars = new jvalue[MAX(1, count)];

            // This version executes fewer instructions but is larger
            // overall.  Seems to be a teensy bit faster.
            assert((vdst >> 16) == 0);  // 16 bits -or- high 16 bits clear
            if (methodToCallType != METHOD_STATIC) {
                thisPtr = (jobject) GET_REGISTER(vdst & 0x0f);
                var_i_reg = 1;
                vdst >>= 4;
            }
            for (; var_i_reg < count && var_i_reg < 4; var_i_reg++, var_i++, vdst >>= 4) {
                switch (methodToCallShorty[var_i + 1]) {
                    case 'D':
                        vars[var_i].d = GET_REGISTER_DOUBLE(vdst & 0x0f);
                        vdst >>= 4;
                        var_i_reg++;
                        break;

                    case 'J':
                        vars[var_i].j = GET_REGISTER_WIDE(vdst & 0x0f);
                        vdst >>= 4;
                        var_i_reg++;
                        break;

                    case '[':
                        while (methodToCallShorty[var_i + 1]) {
                            var_i++;
                        }
                        // not break;
                    default:
                        memcpy(vars + var_i, &(GET_REGISTER(vdst & 0x0f)), sizeof(u4));
                        break;
                }
            }
        }

        // TODO: call method.
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

                case '[':
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
                    LOGE("error method's return type...");
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

                case '[':
                case 'L':
                    LOGD("%p", methodToCallClazz);
                    LOGD("%s", getClassObjectByJClass(methodToCallClazz)->descriptor);
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
                                                     vars);
                    break;

                default:
                    LOGE("error method's return type...");
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

                case '[':
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
                    LOGE("error method's return type...");
                    dvmThrowRuntimeException("error type of field... cc");
                    break;
            }
        }
        // debug print invoke method
        debugInvokeMethod(reinterpret_cast<const Method *>(methodToCall), retval, vars);
        delete[]vars;

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
        LOGD("Handling exception %s at %s",
             getClassObjectByJObject(curException)->descriptor, curMethod->name);

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
        LOGD("dvmFindCatchBlock: %d", catchRelPC);
        pc = curInsns + catchRelPC;
        FINISH(0);
    }
    GOTO_TARGET_END
}

void
dvmCallMethod(JNIEnv *env, jobject instance, const Method *method, const u2 *insns, jvalue *pResult,
              ...) {
    // 参数入寄存器
    int verifyCount = 0;
    const char *desc = &(method->shorty[1]); // [0] is the return type.
    u4 *reg = (u4 *) malloc(sizeof(u4) * method->registersSize);
    u4 *ins = reg + (method->registersSize - method->insSize);

    if (!dvmIsStaticMethod(method)) {
        *ins++ = (u4) instance;
        verifyCount++;
    }

    va_list args;
    va_start(args, pResult);
    while (*desc != '\0') {
        switch (*(desc++)) {
            case 'D':
            case 'J': {
                u8 val = va_arg(args, u8);
                memcpy(ins, &val, 8);       // EABI prevents direct store
                ins += 2;
                verifyCount += 2;
                break;
            }
            case 'F': {
                /* floats were normalized to doubles; convert back */
                float f = (float) va_arg(args, double);
                *ins++ = dvmFloatToU4(f);
                verifyCount++;
                break;
            }
            case 'L': {     /* 'shorty' descr uses L for all refs, incl array */
                void *arg = va_arg(args, void *);
                auto argObj = reinterpret_cast<jobject>(arg);
                *ins++ = (u4) argObj;
                verifyCount++;
                break;
            }
            default: {
                /* Z B C S I -- all passed as 32-bit integers */
                *ins++ = va_arg(args, u4);
                verifyCount++;
                break;
            }
        }
    }
    va_end(args);

    if (verifyCount != method->insSize) {
        LOGE("Got vfycount=%d insSize=%d for %s", verifyCount, method->insSize, method->name);
        GOTO_bail();
    }

    // interpret
    dvmInterpret(env, instance, method, reg, insns, pResult);

    bail:
    free(reg);
}

inline u4 dvmFloatToU4(float val) {
    union {
        float in;
        u4 out;
    } conV;
    conV.in = val;
    return conV.out;
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
jstring dvmResolveString(const ClassObject *referrer, u4 stringIdx) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    const DexStringId *pStringId = dexGetStringId(pDexFile, stringIdx);
    const char *stringData = dexGetStringData(pDexFile, pStringId);

    LOGD("+++ resolving string=%s, referrer is %s", stringData, referrer->descriptor);
    return (*env).NewStringUTF(stringData);
}

jclass dvmResolveClass(const ClassObject *referrer, u4 classIdx) {
    return dvmResolveClass(referrer, classIdx, nullptr);
}

void dvmThrowClassCastException(jclass actual, jclass desired) {
    string msg = getClassNameByJClass(actual);
    msg += " cannot be cast to ";
    msg += getClassNameByJClass(desired);
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
//
//bool dvmIsArrayClass(const jclass arrayClazz) {
//    string clazzName = getClassNameByJClass(arrayClazz);
//    auto len = clazzName.size();
//    return len - 2 >= 0 && clazzName[len - 2] == '[' && clazzName[len - 1] == ']';
//}

string getClassNameByJClass(jclass clazz) {
    return getClassObjectByJClass(clazz)->descriptor;
}

jclass dvmResolveClass(const ClassObject *referrer, u4 classIdx, string *clazzNameString) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    const DexTypeId *pTypeId = dexGetTypeId(pDexFile, classIdx);
    const DexStringId *pStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    string tmp = dexGetStringData(pDexFile, pStringId);
    auto tmpLen = tmp.size();
    if (tmpLen > 0 && tmp[0] == 'L') {
        tmp = tmp.substr(1, tmpLen - 2);
    }
    if (clazzNameString != nullptr) {
        *clazzNameString = tmp;
    }
    LOGD("--- resolving class %s (idx=%u referrer=%s cl=%p)", tmp.data(), classIdx,
         referrer->descriptor, referrer->classLoader);
    return (*env).FindClass(tmp.data());
}

//bool dvmIsArrayClass(const string clazzName) {
//    return clazzName.size() > 0 && clazzName[0] == '[';
//}

jmethodID dvmResolveMethod(const ClassObject *referrer, u4 methodIdx, MethodType methodType,
                           jclass *methodToCallClazz) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    const DexMethodId *pMethodId = dexGetMethodId(pDexFile, methodIdx);
    jclass resClass = dvmResolveClass(referrer, pMethodId->classIdx);
    if (resClass == nullptr) {
        return nullptr;
    }
    *methodToCallClazz = resClass;
    const char *name = dexStringById(pDexFile, pMethodId->nameIdx);
    LOGD("--- resolving method=%s (idx=%u referrer=%s)", name, methodIdx, referrer->descriptor);
    string sign;
    dvmResolveMethodSign(referrer, pMethodId, &sign);
    if (methodType == METHOD_STATIC) {
        return (*env).GetStaticMethodID(resClass, name, sign.data());
    }
    return (*env).GetMethodID(resClass, name, sign.data());
}

void dvmResolveMethodSign(const ClassObject *referrer, const DexMethodId *pMethodId,
                          string *methodSign) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    *methodSign = "(";
    const DexProtoId *pProtoId = dexGetProtoId(pDexFile, pMethodId->protoIdx);
    const DexTypeList *pTypeList = dexGetProtoParameters(pDexFile, pProtoId);
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
    pTypeId = dexGetTypeId(pDexFile, pProtoId->returnTypeIdx);
    pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    const char *returnDescriptor = dexGetStringData(pDexFile, pDexStringId);
    *methodSign += returnDescriptor;

    pTypeId = dexGetTypeId(pDexFile, pMethodId->classIdx);
    pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    const char *clazzName = dexGetStringData(pDexFile, pDexStringId);
    LOGD("--- resolving method sign %s (clazz=%s)", methodSign->data(), clazzName);
}

bool checkForNull(jobject obj) {
    if (obj == nullptr) {
        dvmThrowNullPointerException(nullptr);
        return false;
    }
    return true;
}

// get the ClassObject by fake field
volatile bool fakeGetFieldID = false;

InstField *(*sysDvmFindInstanceFieldHier)(const ClassObject *clazz, const char *fieldName,
                                          const char *signature) = nullptr;


InstField *
fakeDvmFindInstanceFieldHier(const ClassObject *clazz, const char *fieldName,
                             const char *signature) {
    if (fakeGetFieldID && strcmp("00", signature) == 0) {
        fakeGetFieldID = false;
        return (InstField *) clazz;
    }
    return sysDvmFindInstanceFieldHier(clazz, fieldName, signature);
}

void hookDvmFindInstanceFieldHier() {
    static bool flag = false;
    if (!flag) {
        HookNativeInline("/system/lib/libdvm.so",
                         "_Z24dvmFindInstanceFieldHierPK11ClassObjectPKcS3_",
                         reinterpret_cast<void *>(fakeDvmFindInstanceFieldHier),
                         reinterpret_cast<void **>(&sysDvmFindInstanceFieldHier));
        flag = true;
    }
}

ClassObject *getClassObjectByJClass(jclass clazz) {
    fakeGetFieldID = true;
    return reinterpret_cast<ClassObject *>((*env).GetFieldID(clazz, "", "00"));
//    ClassObject* r = nullptr;
//    getObjectOrThreadByJObject(clazz, reinterpret_cast<Object **>(&r), nullptr);
//    return r;
}

ClassObject *getClassObjectByJObject(jobject obj) {
    return getClassObjectByJClass((*env).GetObjectClass(obj));
//    Object* r = nullptr;
//    getObjectOrThreadByJObject(obj, &r, nullptr);
//    return r->clazz;
}

//volatile bool fakeGetObjectRefType = false;
//Object* gObject = nullptr;
//Thread* gThread = nullptr;
//Object* (*sysDvmDecodeIndirectRef)(Thread* self, jobject jobj) = nullptr;
//Object* fakeDvmDecodeIndirectRef(Thread* self, jobject jobj){
//    if(fakeGetObjectRefType){
//        gObject = sysDvmDecodeIndirectRef(self, jobj);
//        fakeGetObjectRefType = false;
//        gThread = self;
//        return gObject;
//    } else{
//        return sysDvmDecodeIndirectRef(self, jobj);
//    }
//}
//
//void hookDvmDecodeIndirectRef() {
//    static bool flag = false;
//    if (!flag) {
//        HookNativeInline("/system/lib/libdvm.so",
//                         "_Z20dvmDecodeIndirectRefP6ThreadP8_jobject",
//                         reinterpret_cast<void *>(fakeDvmDecodeIndirectRef),
//                         reinterpret_cast<void **>(&sysDvmDecodeIndirectRef));
//        flag = true;
//    }
//}
//
//void getObjectOrThreadByJObject(jobject obj, Object **ppObject, Thread **ppThread) {
//    if(ppObject!= nullptr || ppThread!= nullptr){
//        fakeGetObjectRefType = true;
//        (*env).GetObjectRefType(obj);
//        if(ppObject!= nullptr){
//            *ppObject = gObject;
//        } else{
//            *ppThread = gThread;
//        }
//    }
//}

jarray dvmAllocArrayByClass(const s4 length, const ClassObject *referrer, u4 classIdx) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    const DexTypeId *pTypeId = dexGetTypeId(pDexFile, classIdx);
    const DexStringId *pStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    string tmp = dexGetStringData(pDexFile, pStringId);
    LOGD("--- resolving class %s (idx=%u referrer=%s cl=%p)", tmp.data(), classIdx,
         referrer->descriptor, referrer->classLoader);

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
            LOGE("Unknown primitive type '%s'", tmp.data() + 1);
            dvmThrowRuntimeException("error type of field... cc");
            return nullptr;
    }


}

bool dvmInterpHandleFillArrayData(jarray arrayObj, const u2 *arrayData) {
    if (arrayObj == nullptr) {
        dvmThrowNullPointerException(nullptr);
        return false;
    }

    ClassObject *pClassObject = getClassObjectByJObject(arrayObj);
    assert(!IS_CLASS_FLAG_SET(pClassObject, CLASS_ISOBJECTARRAY));
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

//    u2 width = arrayData[1];
    u4 size = arrayData[2] | (((u4) arrayData[3]) << 16);
    if (size > (*env).GetArrayLength(arrayObj)) {
        dvmThrowArrayIndexOutOfBoundsException((*env).GetArrayLength(arrayObj), size);
        return false;
    }

    switch (pClassObject->elementClass->descriptor[0]) {
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
            LOGE("Unknown primitive type '%c'", pClassObject->elementClass->descriptor[0]);
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

s4 dvmInterpHandlePackedSwitch(const u2 *switchData, s4 testVal) {
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
        LOGD("Value %d not found in switch (%d-%d)",
             testVal, firstKey, firstKey + size - 1);
        return kInstrLen;
    }

    /* The entries are guaranteed to be aligned on a 32-bit boundary;
     * we can treat them as a native int array.
     */
    const s4 *entries = (const s4 *) switchData;
    assert(((u4) entries & 0x3) == 0);

    assert(index >= 0 && index < size);
    LOGD("Value %d found in slot %d (goto 0x%02x)",
         testVal, index,
         s4FromSwitchData(&entries[index]));
    return s4FromSwitchData(&entries[index]);
}

s4 dvmInterpHandleSparseSwitch(const u2 *switchData, s4 testVal) {
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
    assert(((u4) keys & 0x3) == 0);

    /* The entries are guaranteed to be aligned on a 32-bit boundary;
     * we can treat them as a native int array.
     */
    entries = keys + size;
    assert(((u4) entries & 0x3) == 0);

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
            LOGD("Value %d found in entry %d (goto 0x%02x)",
                 testVal, mid, s4FromSwitchData(&entries[mid]));
            return s4FromSwitchData(&entries[mid]);
        }
    }

    LOGD("Value %d not found in switch", testVal);
    return kInstrLen;
}

void dvmThrowIncompatibleClassChangeError(const char *msg) {
    dvmThrowNew("java/lang/IncompatibleClassChangeError", msg);
}

bool dvmCanPutArrayElement(const jobject obj, const jobject arrayObj) {
    const ClassObject *objectClass = getClassObjectByJObject(obj);
    const ClassObject *arrayClass = getClassObjectByJObject(arrayObj);
    return strcmp(arrayClass->descriptor + 1, objectClass->descriptor) == 0;
}

void dvmThrowArrayStoreExceptionIncompatibleElement(const jobject obj, const jobject arrayObj) {
    char msg[BUFSIZ];
    memset(msg, 0, BUFSIZ);
    sprintf(msg, "%s cannot be stored in an array of type %s",
            getClassObjectByJObject(obj)->descriptor,
            getClassObjectByJObject(arrayObj)->descriptor);
    dvmThrowNew("java/lang/ArrayStoreException", msg);
}

bool dvmResolveField(const ClassObject *referrer, u4 ifieldIdx, jobject obj, s8 *res,
                     const char **ppName) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    LOGD("--- resolving field %u (referrer=%s cl=%p)",
         ifieldIdx, referrer->descriptor, referrer->classLoader);
    const DexFieldId *pFieldId = dexGetFieldId(pDexFile, ifieldIdx);
    jclass resClass = dvmResolveClass(referrer, pFieldId->classIdx);
    if (resClass == nullptr) {
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
        return false;
    }

    if (obj == nullptr) {
        LOGD("    field %u is %s.%s", ifieldIdx, ((StaticField *) resField)->clazz->descriptor,
             ((StaticField *) resField)->name);
    } else {
        LOGD("    field %u is %s.%s", ifieldIdx, ((InstField *) resField)->clazz->descriptor,
             ((InstField *) resField)->name);
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
            LOGE("error type of field...");
            dvmThrowRuntimeException("error type of field... cc");
            return false;
    }
    if (ppName != nullptr) {
        *ppName = name;
    }
    return true;
}

bool dvmResolveSetField(const ClassObject *referrer, u4 ifieldIdx, jobject obj, u8 res,
                        const char **ppName) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    LOGD("--- resolving field %u (referrer=%s cl=%p)",
         ifieldIdx, referrer->descriptor, referrer->classLoader);
    const DexFieldId *pFieldId = dexGetFieldId(pDexFile, ifieldIdx);
    jclass resClass = dvmResolveClass(referrer, pFieldId->classIdx);
    if (resClass == nullptr) {
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
        return false;
    }

    if (obj == nullptr) {
        LOGD("    field %u is %s.%s", ifieldIdx, ((StaticField *) resField)->clazz->descriptor,
             ((StaticField *) resField)->name);
    } else {
        LOGD("    field %u is %s.%s", ifieldIdx, ((InstField *) resField)->clazz->descriptor,
             ((InstField *) resField)->name);
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
            LOGE("error type of field...");
            dvmThrowRuntimeException("error type of field... cc");
            return false;
    }
    if (ppName != nullptr) {
        *ppName = name;
    }
    return true;
}

void debugInvokeMethod(const Method *method, const jvalue retVal, const jvalue *vars) {
    LOGD("invoke method: %s .%s    %s", method->clazz->descriptor, method->name, method->shorty);

    switch (method->shorty[0]) {
        case 'I':
            LOGD("return value (int): %d", retVal.i);
            break;

        case 'Z':
            retVal.z ? LOGD("return value (bool): true") : LOGD("return value (bool): false");
            break;

        case 'B':
            LOGD("return value (byte): 0x%02x", retVal.b);
            break;

        case 'S':
            LOGD("return value (short): %d", retVal.s);
            break;

        case 'C':
            LOGD("return value (char): %c", retVal.c);
            break;

        case 'F':
            LOGD("return value (float): %f", retVal.f);
            break;

        case 'L':
            LOGD("return value (object): %p", retVal.l);
            break;

        case 'J':
            LOGD("return value (long): %lld", retVal.j);
            break;

        case 'D':
            LOGD("return value (double): %lf", retVal.d);
            break;

        case 'V':
            LOGD("return value (void)");
            break;

        default:
            LOGE("error method's return type...");
            assert(false);
            break;
    }

    for (int var_i = 0; method->shorty[var_i + 1] != '\0'; var_i++) {
        switch (method->shorty[var_i + 1]) {
            case 'I':
                LOGD("var(0x%02x) value (int): %d", var_i, vars[var_i].i);
                break;

            case 'Z':
                vars[var_i].z ? LOGD("var(0x%02x) value (bool): true", var_i) : LOGD(
                        "var(0x%02x) value (bool): false", var_i);
                break;

            case 'B':
                LOGD("var(0x%02x) value (byte): 0x%02x", var_i, vars[var_i].b);
                break;

            case 'S':
                LOGD("var(0x%02x) value (short): %d", var_i, vars[var_i].s);
                break;

            case 'C':
                LOGD("var(0x%02x) value (char): %c", var_i, vars[var_i].c);
                break;

            case 'F':
                LOGD("var(0x%02x) value (float): %f", var_i, vars[var_i].f);
                break;

            case 'L':
                LOGD("var(0x%02x) value (object): %p", var_i, vars[var_i].l);
                break;

            case 'J':
                LOGD("var(0x%02x) value (long): %lld", var_i, vars[var_i].j);
                break;

            case 'D':
                LOGD("var(0x%02x) value (double): %lf", var_i, vars[var_i].d);
                break;

            default:
                LOGE("error method's return type...");
                assert(false);
                break;
        }
    }
}

int dvmFindCatchBlock(const Method *method, size_t pcOff, jobject exception) {
    const DexFile *pDexFile = method->clazz->pDvmDex->pDexFile;
    const DexCode *pDexCode = dvmGetMethodCode(method);
    const DexTry *pDexTry = dexGetTries(pDexCode);
    const u1 *catchHandlerList = dexGetCatchHandlerData(pDexCode);

    const DexTry *pRetDexTry = nullptr;
    int catchOff = 0;
    // find try
    for (int i = 0; i < pDexCode->triesSize; i++, pDexTry++) {
        if (pDexTry->startAddr <= pcOff && pcOff < pDexTry->startAddr + pDexTry->insnCount &&
            (pRetDexTry == nullptr || (pRetDexTry->startAddr < pDexTry->startAddr))) {
            const u1 *pCatchHandler = catchHandlerList + pDexTry->handlerOff;
            int size = readSignedLeb128(&pCatchHandler);
            // find catch
            bool catchFlag = false;
            for (int j = 0; j < abs(size); j++) {
                u4 type_idx = static_cast<u4>(readUnsignedLeb128(&pCatchHandler));
                int addr = readUnsignedLeb128(&pCatchHandler);

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
                    catchOff = addr;
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
void dvmThrowVerificationError(const Method *method, int kind, int ref) {
    int errorPart = kind & ~(0xff << kVerifyErrorRefTypeShift);
    int errorRefPart = kind >> kVerifyErrorRefTypeShift;
    VerifyError errorKind = static_cast<VerifyError>(errorPart);
    VerifyErrorRefType refType = static_cast<VerifyErrorRefType>(errorRefPart);
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


string classNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags) {
    const DvmDex *pDvmDex = method->clazz->pDvmDex;
    if (refType == VERIFY_ERROR_REF_FIELD) {
        /* get class ID from field ID */
        const DexFieldId *pFieldId = dexGetFieldId(pDvmDex->pDexFile, static_cast<u4>(ref));
        ref = pFieldId->classIdx;
    } else if (refType == VERIFY_ERROR_REF_METHOD) {
        /* get class ID from method ID */
        const DexMethodId *pMethodId = dexGetMethodId(pDvmDex->pDexFile, static_cast<u4>(ref));
        ref = pMethodId->classIdx;
    }

    const char *className = dexStringByTypeIdx(pDvmDex->pDexFile, static_cast<u4>(ref));
    string dotClassName(dvmHumanReadableDescriptor(className));
    if (flags == 0) {
        return dotClassName;
    }

    string result;
    if ((flags & kThrowShow_accessFromClass) != 0) {
        result += "tried to access class " + dotClassName;
        result += " from class " + dvmHumanReadableDescriptor(method->clazz->descriptor);
    } else {
        assert(false);      // should've been caught above
    }

    return result;
}

string fieldNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags) {
    if (refType != VERIFY_ERROR_REF_FIELD) {
        LOGE("Expected ref type %d, got %d", VERIFY_ERROR_REF_FIELD, refType);
        return nullptr;    /* no message */
    }

    const DvmDex *pDvmDex = method->clazz->pDvmDex;
    const DexFieldId *pFieldId = dexGetFieldId(pDvmDex->pDexFile, static_cast<u4>(ref));
    const char *className = dexStringByTypeIdx(pDvmDex->pDexFile, pFieldId->classIdx);
    const char *fieldName = dexStringById(pDvmDex->pDexFile, pFieldId->nameIdx);

    string dotName(dvmHumanReadableDescriptor(className));

    if ((flags & kThrowShow_accessFromClass) != 0) {
        string result;
        result += "tried to access field ";
        result += dotName + "." + fieldName;
        result += " from class ";
        result += dvmHumanReadableDescriptor(method->clazz->descriptor);
        return result;
    }
    return dotName + "." + fieldName;
}

string methodNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags) {
    if (refType != VERIFY_ERROR_REF_METHOD) {
        LOGE("Expected ref type %d, got %d", VERIFY_ERROR_REF_METHOD, refType);
        return nullptr;    /* no message */
    }

    const DvmDex *pDvmDex = method->clazz->pDvmDex;
    const DexMethodId *pMethodId = dexGetMethodId(pDvmDex->pDexFile, static_cast<u4>(ref));
    const char *className = dexStringByTypeIdx(pDvmDex->pDexFile, pMethodId->classIdx);
    const char *methodName = dexStringById(pDvmDex->pDexFile, pMethodId->nameIdx);

    string dotName(dvmHumanReadableDescriptor(className));

    if ((flags & kThrowShow_accessFromClass) != 0) {
        string desc;
        dvmResolveMethodSign(method->clazz, &method->prototype, &desc);
        string result;
        result += "tried to access method ";
        result += dotName + "." + methodName + ":" + desc;
        result += " from class " + dvmHumanReadableDescriptor(method->clazz->descriptor);
        return result;
    }
    return dotName + "." + methodName;
}

void
dvmResolveMethodSign(const ClassObject *referrer, const DexProto *pDexProto, string *methodSign) {
    DexFile *pDexFile = referrer->pDvmDex->pDexFile;
    *methodSign = "(";
    const DexProtoId *pProtoId = dexGetProtoId(pDexFile, pDexProto->protoIdx);
    const DexTypeList *pTypeList = dexGetProtoParameters(pDexFile, pProtoId);
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
    pTypeId = dexGetTypeId(pDexFile, pProtoId->returnTypeIdx);
    pDexStringId = dexGetStringId(pDexFile, pTypeId->descriptorIdx);
    const char *returnDescriptor = dexGetStringData(pDexFile, pDexStringId);
    *methodSign += returnDescriptor;
}

// hook test demo
bool (*sysDvmLinkClass)(ClassObject *clazz);

const void *gInsns = nullptr;

bool myDvmLinkClass(ClassObject *clazz) {
    bool r = sysDvmLinkClass(clazz);
    LOGD("clazz->descriptor: %s", clazz->descriptor);
    if (strcmp(clazz->descriptor, "Lcom/chend/vmdemo/MainActivity;") == 0) {
        Method *test1;
        Method *test2;
        for (int i = 0; i < clazz->virtualMethodCount; i++) {
            if (strcmp("test1", clazz->virtualMethods[i].name) == 0) {
                test1 = &clazz->virtualMethods[i];
                LOGD("test1 ok");
            } else if (strcmp("test2", clazz->virtualMethods[i].name) == 0) {
                test2 = &clazz->virtualMethods[i];
                LOGD("test2 ok");
            }
        }
        LOGD("test1 insns: %p", test1->insns);
        gInsns = test1->insns;

        LOGD("jniArgInfo: %d %d", test1->jniArgInfo, test2->jniArgInfo);
        LOGD("accessFlags: %d %d", test1->accessFlags, test2->accessFlags);
        LOGD("outsSize: %d %d", test1->outsSize, test2->outsSize);
        LOGD("insSize: %d %d", test1->insSize, test2->insSize);
        LOGD("registersSize: %d %d", test1->registersSize, test2->registersSize);
        LOGD("fastJni: %d %d", test1->fastJni, test2->fastJni);

        test1->nativeFunc = test2->nativeFunc;
        test1->jniArgInfo = test2->jniArgInfo;
        test1->accessFlags = test2->accessFlags;
        test1->outsSize = test2->outsSize;
        test1->insSize = test2->insSize;
        test1->registersSize = test2->registersSize;
        test1->fastJni = test2->fastJni;

//        gInsns = test1->insns;


    }
    return r;
}

void hook() {
    static bool flag = false;
    if (!flag) {
        HookNativeInline("/system/lib/libdvm.so", "_Z12dvmLinkClassP11ClassObject",
                         reinterpret_cast<void *>(myDvmLinkClass),
                         reinterpret_cast<void **>(&sysDvmLinkClass));
        flag = true;
    }
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_ERR;
    }
    hookDvmFindInstanceFieldHier();
//    hookDvmDecodeIndirectRef();
    LOGD("jni_on_load");

    hook();
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_chend_vmdemo_MainActivity_test1(JNIEnv *env, jobject instance, jfloat a, jdouble b) {

    LOGD("test1s..");

    jclass jclass1 = (*env).GetObjectClass(instance);
    jmethodID jmethodID1 = (*env).GetMethodID(jclass1, "test1", "(FD)J");
    Method methodImg = *(Method *) jmethodID1;
    Method *method = &methodImg;
    method->insns = static_cast<const u2 *>(gInsns);

    const u1 insns[] = {0x13, 0x00, 0x0a, 0x00, 0x23, 0x01, 0x60, 0x08, 0x5b, 0x71, 0x52, 0x25,
                        0x12, 0x01, 0x35, 0x01, 0x0e, 0x00, 0x54, 0x72, 0x52, 0x25, 0x22, 0x03,
                        0x67, 0x07, 0x70, 0x20, 0x81, 0x3c, 0x13, 0x00, 0x4d, 0x03, 0x02, 0x01,
                        0xd8, 0x01, 0x01, 0x01, 0x28, 0xf3, 0x54, 0x71, 0x52, 0x25, 0x12, 0x32,
                        0x46, 0x03, 0x01, 0x02, 0x38, 0x03, 0x0d, 0x00, 0x46, 0x03, 0x01, 0x02,
                        0x20, 0x33, 0x67, 0x07, 0x38, 0x03, 0x07, 0x00, 0x46, 0x01, 0x01, 0x02,
                        0x6e, 0x10, 0x77, 0x3c, 0x01, 0x00, 0x13, 0x01, 0x1a, 0x00, 0x23, 0x12,
                        0x1d, 0x08, 0x5b, 0x72, 0x51, 0x25, 0x12, 0x02, 0x35, 0x12, 0x09, 0x00,
                        0x54, 0x73, 0x51, 0x25, 0x4b, 0x02, 0x03, 0x02, 0xd8, 0x02, 0x02, 0x01,
                        0x28, 0xf8, 0x23, 0x01, 0x1c, 0x08, 0x5b, 0x71, 0x53, 0x25, 0x23, 0x01,
                        0x1a, 0x08, 0x5b, 0x71, 0x54, 0x25, 0x23, 0x00, 0x19, 0x08, 0x69, 0x00,
                        0x55, 0x25, 0x54, 0x70, 0x51, 0x25, 0x21, 0x01, 0x12, 0x02, 0x12, 0x23,
                        0x35, 0x12, 0x28, 0x00, 0x44, 0x04, 0x00, 0x02, 0xdc, 0x05, 0x04, 0x03,
                        0x38, 0x05, 0x18, 0x00, 0x12, 0x16, 0x32, 0x65, 0x0d, 0x00, 0x32, 0x35,
                        0x03, 0x00, 0x28, 0x18, 0x62, 0x03, 0x55, 0x25, 0xdb, 0x05, 0x04, 0x03,
                        0x8d, 0x46, 0x4f, 0x06, 0x03, 0x05, 0x28, 0x10, 0x54, 0x73, 0x54, 0x25,
                        0xdb, 0x05, 0x04, 0x03, 0x8e, 0x46, 0x50, 0x06, 0x03, 0x05, 0x28, 0x08,
                        0x54, 0x73, 0x53, 0x25, 0xdb, 0x05, 0x04, 0x03, 0x82, 0x46, 0x4b, 0x06,
                        0x03, 0x05, 0xd8, 0x02, 0x02, 0x01, 0x28, 0xd8, 0x54, 0x70, 0x53, 0x25,
                        0x44, 0x00, 0x00, 0x03, 0x7f, 0x00, 0x15, 0x01, 0xa0, 0x40, 0x2e, 0x00,
                        0x00, 0x01, 0x3b, 0x00, 0x0f, 0x00, 0x62, 0x00, 0x84, 0x2c, 0x6f, 0x10,
                        0x66, 0x3d, 0x07, 0x00, 0x0c, 0x01, 0x6e, 0x10, 0x0c, 0x3d, 0x01, 0x00,
                        0x0c, 0x01, 0x6e, 0x20, 0xe6, 0x3c, 0x10, 0x00, 0x62, 0x00, 0x55, 0x25,
                        0x48, 0x00, 0x00, 0x03, 0x81, 0x00, 0x16, 0x02, 0xff, 0xff, 0xc2, 0x02,
                        0x62, 0x04, 0x84, 0x2c, 0x6e, 0x30, 0xe5, 0x3c, 0x24, 0x03, 0x16, 0x04,
                        0xff, 0x00, 0x10, 0x04};

    method->registersSize = 0xb;
    method->insSize = 0x4;

    jvalue value;
    dvmCallMethod(env, instance, method, reinterpret_cast<const u2 *>(insns), &value, a, b);
    return value.j;
}