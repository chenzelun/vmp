import shutil
from struct import pack
from typing import List
from xml.etree import ElementTree
from zipfile import ZipFile

from dex_file import JNIFuncBuilder
from env_settings import *
from util.AXMLUtil.axml import AndroidXML
from util.apk.apk import APK
from util.apktool.apktool import ApkTool


class Shell:
    REBUILD_SMALI_FILES_NAME = [
        'ShellApplication.smali'
    ]

    SHELL_DEPENDENCIES_JAR_PACKAGE_NAME = [
    ]

    log = logging.getLogger(__name__)

    def __init__(self):
        # src apk
        self.in_apk_name: str = ''
        self.in_apk_path: str = ''
        self.in_apk_pkg_name: str = ''
        self.in_apk_application_name: str = ''
        in_apk: str = os.listdir(APK_IN)[0]
        self.in_apk_name: str = in_apk[:-4]
        self.in_apk_path: str = APK_IN + '/' + in_apk

        # shell apk
        self.shell_apk_pkg_name: str = ''
        self.shell_apk_name: str = ''
        self.shell_apk_path: str = ''
        self.shell_apk_name: str = ''

        self.tmp_shell_path: str = ''

        # tmp files
        self.tmp_in_path: str = ''
        self.tmp_out_path: str = ''

        # file name
        self.in_classes_dex_file_name: str = 'classes.dex'
        self.code_item_file_name: str = 'code_items.bin'
        self.method_insns_file_name: str = 'method_insns.bin'
        self.jni_func_file_name: str = 'JNIFunc.cpp'
        self.jni_func_header_file_name: str = 'JNIFunc.h'
        self.config_file_name: str = 'config.bin'
        self.fake_classes_dex_file_name: str = '/fake.dex'
        self.fake_classes_dex_file_buf: List[int] = [
            0x64, 0x65, 0x78, 0x0A, 0x30, 0x33, 0x35, 0x00, 0xD9, 0x24, 0x14, 0xFD, 0x2F, 0x81, 0x4D, 0x8B,
            0x50, 0x48, 0x13, 0x1D, 0x8D, 0xA9, 0xCF, 0x1F, 0xF1, 0xF2, 0xDD, 0x06, 0xB4, 0x67, 0x70, 0xA1,
            0x24, 0x01, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xD8, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00,
            0xA4, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x0E, 0x4C, 0x63, 0x6F, 0x6D, 0x2F, 0x6D, 0x69, 0x78, 0x43, 0x6C, 0x61,
            0x73, 0x73, 0x3B, 0x00, 0x12, 0x4C, 0x6A, 0x61, 0x76, 0x61, 0x2F, 0x6C, 0x61, 0x6E, 0x67, 0x2F,
            0x4F, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x3B, 0x00, 0x0D, 0x6D, 0x69, 0x78, 0x43, 0x6C, 0x61, 0x73,
            0x73, 0x2E, 0x6A, 0x61, 0x76, 0x61, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
            0x70, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00,
            0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00,
            0x03, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0xD8, 0x00, 0x00, 0x00
        ]

    def update_shell_apk(self):
        # shell apk
        self.shell_apk_pkg_name: str = ''
        self.shell_apk_name: str = os.listdir(APK_SHELL)[0]
        self.shell_apk_path: str = APK_SHELL + '/' + self.shell_apk_name
        self.shell_apk_name: str = self.shell_apk_name[:-4]
        # tmp files
        self.tmp_shell_path: str = APK_TMP_SHELL + '/' + self.shell_apk_name

    def unzip_in_apk(self):
        in_path, in_name = self.in_apk_path, self.in_apk_name
        tmp_in_path = APK_TMP_IN + '/' + in_name
        tmp_out_path = APK_TMP_OUT + '/' + in_name
        with ZipFile(in_path) as apk:
            apk.extractall(tmp_in_path)
            self.log.info(
                'unzip file from {from_path} to {to_path}'.format(from_path=in_path, to_path=tmp_in_path))

        self.tmp_in_path = tmp_in_path
        self.tmp_out_path = tmp_out_path
        os.mkdir(tmp_out_path)
        os.mkdir(tmp_out_path + '/assets')
        self.log.debug('mkdir: %s', tmp_in_path)
        self.log.debug('mkdir: %s', tmp_out_path)
        self.log.debug('mkdir: %s', tmp_out_path + '/assets')
        self.log.info('unzip_in_apk finish...')

    def decompile_shell_apk(self):
        ApkTool.decode(self.shell_apk_path, self.tmp_shell_path, APKTOOL_PATH)
        self.log.info('decompile_shell_apk finish...')

    def modify_in_manifest(self):
        # get shell apk application's name and package's name
        manifest = ElementTree.parse(self.tmp_shell_path + '/AndroidManifest.xml').getroot()
        self.shell_apk_pkg_name = manifest.attrib['package']
        application = manifest.find('application')
        key = '{http://schemas.android.com/apk/res/android}name'
        shell_apk_application_name = application.attrib[key]
        if shell_apk_application_name[0] == '.':
            shell_apk_application_name = shell_apk_application_name[1:]
        else:
            shell_apk_application_name = shell_apk_application_name[len(self.shell_apk_pkg_name) + 1:]

        self.log.debug('shell_apk_application_name: %s', shell_apk_application_name)

        # get the in_apk's application name and modify it to shell application name
        in_path, out_path = self.tmp_in_path, self.tmp_out_path
        in_path += '/AndroidManifest.xml'
        out_path += '/AndroidManifest.xml'

        # read original in_apk's manifest file
        manifest_str = AndroidXML.axml2xml(in_path, AXML_PRINTER_PATH)
        manifest = ElementTree.fromstring(manifest_str[0])
        self.in_apk_pkg_name = manifest.attrib['package']
        application = manifest.find('application')
        if key in application.attrib:
            in_apk_application_name = application.attrib[key]
            if in_apk_application_name[0] == '.':
                self.in_apk_application_name = self.in_apk_pkg_name + in_apk_application_name
            else:
                self.in_apk_application_name = in_apk_application_name
        else:
            self.in_apk_application_name = 'android.app.Application'

        self.log.debug('src package name: ' + self.in_apk_pkg_name)
        self.log.debug('src application name: ' + self.in_apk_application_name)
        in_apk_changed_application_name = self.in_apk_pkg_name + '.' + shell_apk_application_name
        self.log.debug('src changed application name: ' + in_apk_changed_application_name)

        # modify in_apk's application name
        AndroidXML.modify_attr('application', 'package', 'name', in_apk_changed_application_name,
                               in_path, out_path, AXML_EDITOR_PATH)

        # print modified in_apk's manifest
        AndroidXML.print_xml(out_path, AXML_PRINTER_PATH)
        self.log.debug('create manifest: %s', out_path)
        self.log.info('modify_in_manifest finish...')

    def rebuild_shell_dex(self):
        # copy the shell_apk's smali files to modify them later
        tmp_copy_shell_apk_path = self.tmp_shell_path + 'copy'
        shutil.copytree(self.tmp_shell_path, tmp_copy_shell_apk_path)
        self.log.debug('create copy shell dir(decompiled)')

        # the dir path of shell_apk's smali files
        shell_smali_package_name = self.shell_apk_pkg_name.replace('.', '/')
        self.log.debug('shell_smali_package_name: %s', shell_smali_package_name)
        shell_smali_src_path = self.tmp_shell_path + '/smali/' + shell_smali_package_name
        shell_smali_dst_path = tmp_copy_shell_apk_path + '/smali'
        self.log.debug('shell_smali_src_path: %s', shell_smali_src_path)
        self.log.debug('shell_smali_dst_path: %s', shell_smali_dst_path)

        in_name, in_pkg_name, out_path = self.in_apk_name, self.in_apk_pkg_name, self.tmp_out_path
        # remove all original shell_apk's smali files
        shutil.rmtree(shell_smali_dst_path)
        os.mkdir(shell_smali_dst_path)

        # modify the package name in shell_apk's smali files and write them
        src_smali_package_name = in_pkg_name.replace('.', '/')
        self.log.debug('src_smali_package_name: ' + src_smali_package_name)
        for smali_file in self.REBUILD_SMALI_FILES_NAME:
            # read all smali files needed to rebuild
            with open(shell_smali_src_path + '/' + smali_file, 'r') as r:
                smali_data = r.read()

            with open(shell_smali_dst_path + '/' + smali_file, 'w') as w:
                new_smali_data = smali_data.replace(shell_smali_package_name, src_smali_package_name)
                w.write(new_smali_data)
                w.flush()
                self.log.debug('write file: ' + shell_smali_dst_path + '/' + smali_file)
                self.log.debug('smali data: \n' + new_smali_data)

        # copy third-lib's smali
        for lib in self.SHELL_DEPENDENCIES_JAR_PACKAGE_NAME:
            lib_smali_path = lib.replace('.', '/')
            lib_src_path = self.tmp_shell_path + '/smali/' + lib_smali_path
            lib_dst_path = shell_smali_dst_path + '/' + lib_smali_path
            shutil.copytree(lib_src_path, lib_dst_path)
            self.log.debug("copy third-lib's smali files from %s to %s", lib_src_path, lib_dst_path)

        # generate modified shell_apk's classes.dex
        tmp_rebuild_apk = self.tmp_shell_path + '.apk'
        ApkTool.build(tmp_copy_shell_apk_path, tmp_rebuild_apk, APKTOOL_PATH)
        with ZipFile(tmp_rebuild_apk) as apk:
            apk.extract('classes.dex', out_path)
            self.log.debug('create dex: %s/classes.dex', out_path)

        self.log.info('rebuild_shell_dex finish...')

    def encrypt_shell_so(self):
        if not os.path.exists(self.tmp_out_path + '/lib'):
            shutil.copytree(self.tmp_shell_path + '/lib', self.tmp_out_path + '/lib')
        else:
            abis = os.listdir(self.tmp_out_path + '/lib')
            if not abis:
                os.remove(self.tmp_out_path + '/lib')
                shutil.copytree(self.tmp_shell_path + '/lib', self.tmp_out_path + '/lib')
            for abi in abis:
                in_path = self.tmp_shell_path + '/lib/' + abi + '/'
                out_path = self.tmp_out_path + '/lib/' + abi + '/'
                for f in os.listdir(in_path):
                    shutil.copyfile(in_path + '/' + f, out_path + '/' + f)

        self.log.debug('copy files from %s to %s', self.tmp_shell_path + '/lib', self.tmp_out_path + '/lib')
        self.log.info('encrypt_shell_so finish...')

    def encrypt_in_so(self):
        if os.path.exists(self.tmp_in_path + '/lib'):
            shutil.copytree(self.tmp_in_path + '/lib', self.tmp_out_path + '/lib')

    def encrypt_in_dex(self):
        in_path, in_pkg_name, out_path = self.tmp_in_path, self.in_apk_pkg_name, self.tmp_out_path
        out_path += '/assets'

        manifest_str = AndroidXML.axml2xml(self.tmp_in_path + '/AndroidManifest.xml', AXML_PRINTER_PATH)
        manifest = ElementTree.fromstring(manifest_str[0])
        in_pkg_name = manifest.attrib['package']
        exclude_class_names = {
            'BuildConfig',
            'R$anim',
            'R$attr',
            'R$bool',
            'R$color',
            'R$dimen',
            'R$drawable',
            'R$id',
            'R$integer',
            'R$layout',
            'R$mipmap',
            'R$string',
            'R$style',
            'R$styleable',
            'R',
        }
        pkg_clazz_path = 'L' + in_pkg_name.replace('.', '/') + '/'
        exclude_class_names = {(pkg_clazz_path + x + ';') for x in exclude_class_names}

        # TODO Test
        include_pkg_names = [in_pkg_name]

        builder = JNIFuncBuilder(in_path + '/classes.dex')
        builder.filter_methods(include_pkg_names=include_pkg_names, exclude_class_names=exclude_class_names)
        builder.write_to(code_item_out_path=out_path + '/' + self.code_item_file_name,
                         method_insns_out_path=out_path + '/' + self.method_insns_file_name,
                         dex_out_path=out_path + '/' + self.in_classes_dex_file_name,
                         jni_func_out_path=out_path + '/' + self.jni_func_file_name,
                         jni_func_header_out_path=out_path + '/' + self.jni_func_header_file_name)
        self.log.debug('create file: %s', out_path + '/' + self.code_item_file_name)
        self.log.debug('create file: %s', out_path + '/' + self.in_classes_dex_file_name)
        self.log.debug('create file: %s', out_path + '/' + self.jni_func_file_name)
        self.log.debug('create file: %s', out_path + '/' + self.jni_func_header_file_name)
        self.log.info('encrypt_src_dex, finish...')

    def copy_src_other_res(self):
        src_path, out_path = self.tmp_in_path, self.tmp_out_path
        files = set(os.listdir(src_path))
        if 'assets' in files:
            files.remove('assets')
            src = src_path + '/assets'
            dst = out_path + '/assets'
            for f in os.listdir(src):
                shutil.copy(src + '/' + f, dst)
            self.log.debug('copy files from %s to %s' % (src, dst))

        if 'lib' in files:
            files.remove('lib')

        if 'classes.dex' in files:
            files.remove('classes.dex')

        if 'META-INF' in files:
            files.remove('META-INF')

        if 'AndroidManifest.xml' in files:
            files.remove('AndroidManifest.xml')

        for f in files:
            f_path = src_path + '/' + f
            if os.path.isdir(f_path):
                shutil.copytree(f_path, out_path + '/' + f)
            else:
                shutil.copy(f_path, out_path)
            self.log.debug('copy other files from %s to %s' % (f_path, out_path + '/' + f))

        self.log.info('copy_src_other_res finish...')

    def rebuild_and_resign_apk(self):
        out_path, src_apk_name = self.tmp_out_path, self.in_apk_name
        self.write_config_file()

        # rebuild apk
        rebuild_zip_path = shutil.make_archive(out_path, 'zip', out_path, logger=self.log)
        self.log.debug('rebuild_zip_path: ' + rebuild_zip_path)
        rebuild_apk_path = APK_TMP_OUT + '/' + src_apk_name + '.apk'
        os.rename(rebuild_zip_path, rebuild_apk_path)
        self.log.debug('rebuild_apk_path: ' + rebuild_apk_path)

        apktool_decode_path = APK_TMP_OUT + '/' + src_apk_name + '_apktool'
        apktool_rebuild_path = APK_TMP_OUT + '/' + src_apk_name + '_apktool_unsigned.apk'
        ApkTool.decode(rebuild_apk_path, apktool_decode_path, APKTOOL_PATH)
        ApkTool.build(apktool_decode_path, apktool_rebuild_path, APKTOOL_PATH)

        # sign
        out_apk_path = APK_OUT + '/' + src_apk_name + '_signed.apk'
        if os.path.exists(out_apk_path):
            os.remove(out_apk_path)
        APK.sign(KEY_STORE_PATH, KEY_STORE_PWD, KEY_STORE_ALIAS, KEY_STORE_ALIAS_PWD,
                 apktool_rebuild_path, out_apk_path)
        self.log.debug('resign apk success: %s', out_apk_path)
        self.log.info('rebuild_and_resign_apk finish...')

    def remove_all_tmp(self):
        for tmp_dir in [APK_TMP_OUT, APK_TMP_SHELL, APK_TMP_IN]:
            for tmp_file in os.listdir(tmp_dir):
                tmp_abs_path = tmp_dir + '/' + tmp_file
                if os.path.isdir(tmp_abs_path):
                    shutil.rmtree(tmp_abs_path)
                else:
                    os.remove(tmp_abs_path)
        self.log.info('remove_all_tmp finish...')

    def write_config_file(self):
        """
        config file format:
            index:   # index size: 6*2*4B
                application_name_size:                          uint32
                application_name_off:                           uint32

                fake_classes_dex_file_name_size:                uint32
                fake_classes_dex_file_name_off:                 uint32

                fake_classes_dex_file_buf_size:                 uint32
                fake_classes_dex_file_buf_off:                  uint32

                classes_dex_file_size:                          uint32
                classes_dex_file_off:                           uint32

                code_item_file_size:                            uint32
                code_item_file_off:                             uint32

                method_insns_file_size:                         uint32
                method_insns_file_off:                          uint32

            data:
                application_name:                               char[]
                fake_classes_dex_file_name:                     char[]
                fake_classes_dex_buf_name:                      char[]
                classes_dex_file:                               char[]
                code_item_file:                                 char[]
                method_insns_file:                              char[]
        :return:
        """

        index_size = 6 * 2 * 4
        index = bytearray()
        data = bytearray()

        data_cur_offset = index_size + len(data)
        index.extend(pack('<2I', len(self.in_apk_application_name) + 1, data_cur_offset))
        data.extend(bytes(self.in_apk_application_name, encoding='ascii'))
        data.append(0)

        data_cur_offset = index_size + len(data)
        index.extend(pack('<2I', len(self.fake_classes_dex_file_name) + 1, data_cur_offset))
        data.extend(bytes(self.fake_classes_dex_file_name, encoding='ascii'))
        data.append(0)

        data_cur_offset = index_size + len(data)
        index.extend(pack('<2I', len(self.fake_classes_dex_file_buf), data_cur_offset))
        data.extend(self.fake_classes_dex_file_buf)

        data_cur_offset = index_size + len(data)
        with open(self.tmp_out_path + '/assets/' + self.in_classes_dex_file_name, 'rb') as reader:
            dex_buf = reader.read()
        index.extend(pack('<2I', len(dex_buf), data_cur_offset))
        data.extend(dex_buf)

        data_cur_offset = index_size + len(data)
        with open(self.tmp_out_path + '/assets/' + self.code_item_file_name, 'rb') as reader:
            code_item_buf = reader.read()
        index.extend(pack('<2I', len(code_item_buf), data_cur_offset))
        data.extend(code_item_buf)

        data_cur_offset = index_size + len(data)
        with open(self.tmp_out_path + '/assets/' + self.method_insns_file_name, 'rb') as reader:
            method_insns_buf = reader.read()
        index.extend(pack('<2I', len(method_insns_buf), data_cur_offset))
        data.extend(method_insns_buf)

        os.remove(self.tmp_out_path + '/assets/' + self.in_classes_dex_file_name)
        os.remove(self.tmp_out_path + '/assets/' + self.code_item_file_name)
        os.remove(self.tmp_out_path + '/assets/' + self.method_insns_file_name)
        with open(self.tmp_out_path + '/assets/' + self.config_file_name, 'wb') as writer:
            writer.write(index)
            writer.write(data)
            writer.flush()

        self.log.info('write file: %s', self.tmp_out_path + '/assets/' + self.config_file_name)
