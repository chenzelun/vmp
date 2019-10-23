import os
import shutil

from env_settings import APK_OUT, APK_IN, APK_SHELL, APK_TMP_OUT, APK_TMP_SHELL, APK_TMP_IN
from shell import Shell
from util.apk.apk import APK

if __name__ == '__main__':
    # debug
    # clean tmp cache
    for tmp_dir in [APK_TMP_OUT,
                    APK_TMP_SHELL,
                    APK_TMP_IN,
                    APK_OUT,
                    APK_IN,
                    APK_SHELL]:
        for tmp_file in os.listdir(tmp_dir):
            tmp_abs_path = tmp_dir + '/' + tmp_file
            if os.path.isdir(tmp_abs_path):
                shutil.rmtree(tmp_abs_path)
            else:
                os.remove(tmp_abs_path)

    while True:
        # start
        root_path = os.path.dirname(os.path.abspath(os.curdir))
        in_apk_path = root_path + r'\TestApp\app\build\outputs\apk\debug\app-debug.apk'
        shell_app_base_dir = root_path + r'\vmpApplication'
        shell_cpp_path = shell_app_base_dir + r'\app\src\main\cpp'
        shell_apk_path = shell_app_base_dir + r'\app\build\outputs\apk\debug\app-debug.apk'

        # cp in apk
        shutil.copyfile(in_apk_path, APK_IN + r'\app-debug.apk')

        shell = Shell()
        shell.unzip_in_apk()
        shell.encrypt_in_dex()

        # cp the jni_func.cpp and jni_func.h to shell app and rebuild it
        if os.path.exists(shell_cpp_path + '/' + shell.jni_func_file_name):
            os.remove(shell_cpp_path + '/' + shell.jni_func_file_name)
        shutil.move(shell.tmp_out_path + '/assets/' + shell.jni_func_file_name,
                    shell_cpp_path + '/' + shell.jni_func_file_name)
        if os.path.exists(shell_cpp_path + '/' + shell.jni_func_header_file_name):
            os.remove(shell_cpp_path + '/' + shell.jni_func_header_file_name)
        shutil.move(shell.tmp_out_path + '/assets/' + shell.jni_func_header_file_name,
                    shell_cpp_path + '/' + shell.jni_func_header_file_name)

        APK.gradle_build_debug_apk(shell_app_base_dir)
        # cp shell app
        shutil.copyfile(shell_apk_path, APK_SHELL + r'\app-debug.apk')
        shell.update_shell_apk()

        shell.decompile_shell_apk()
        shell.encrypt_in_so()
        shell.encrypt_shell_so()
        shell.modify_in_manifest()
        shell.rebuild_shell_dex()
        shell.copy_src_other_res()
        shell.rebuild_and_resign_apk()

        # debug
        # install the out apk
        APK.install(APK_OUT + '/' + os.listdir(APK_OUT)[0])
        APK.start(shell.in_apk_pkg_name)
        APK.uninstall(shell.in_apk_pkg_name)
        shell.remove_all_tmp()

        # debug
        # clean tmp cache
        for tmp_dir in [APK_TMP_OUT,
                        APK_TMP_SHELL,
                        APK_TMP_IN,
                        APK_OUT,
                        APK_IN,
                        APK_SHELL]:
            for tmp_file in os.listdir(tmp_dir):
                tmp_abs_path = tmp_dir + '/' + tmp_file
                if os.path.isdir(tmp_abs_path):
                    shutil.rmtree(tmp_abs_path)
                else:
                    os.remove(tmp_abs_path)

        print('ok')
