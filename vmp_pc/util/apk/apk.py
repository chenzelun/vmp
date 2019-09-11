import logging
from subprocess import Popen, PIPE

import chardet


class APK:
    logger = logging.getLogger(__name__)

    @staticmethod
    def install(apk_path, log=True):
        child = Popen(
            ['adb', 'install', '-r', '-t', apk_path],
            stdout=PIPE, stderr=PIPE)
        APK.__log(child, log)

    @staticmethod
    def start(pkg_name, log=True):
        child = Popen(
            ['adb', 'shell', 'am', 'start', pkg_name + '/.MainActivity'],
            stdout=PIPE, stderr=PIPE)
        APK.__log(child, log)

    @staticmethod
    def uninstall(pkg_name, log=True):
        child = Popen(
            ['adb', 'uninstall', pkg_name],
            stdout=PIPE, stderr=PIPE)
        APK.__log(child, log)

    @staticmethod
    def gradle_build_debug_apk(cwd, log=True):
        child = Popen(
            [cwd + r'\gradlew.bat', 'clean', 'assembleDebug', '--info', '--stacktrace'],
            stdout=PIPE, stderr=PIPE, cwd=cwd)
        APK.__log(child, log)

    @staticmethod
    def sign(keystore_path, store_pwd, alias, alias_pwd, in_path, out_path, log=True):
        child = Popen(
            ['jarsigner', '-verbose', '-sigalg', 'SHA1withRSA', '-digestalg', 'SHA1',
             '-keystore', keystore_path, '-storepass', store_pwd,
             '-signedjar', out_path, in_path,
             alias, '-keypass', alias_pwd],
            stdout=PIPE, stderr=PIPE)
        APK.__log(child, log)

    @classmethod
    def __log(cls, child, log):
        # noinspection DuplicatedCode
        out_str, err_str = child.communicate()
        if log:
            if out_str:
                cls.logger.debug(str(out_str, encoding=chardet.detect(out_str)['encoding']))
            if err_str:
                cls.logger.error(str(err_str, encoding=chardet.detect(err_str)['encoding']))
        return out_str, err_str
