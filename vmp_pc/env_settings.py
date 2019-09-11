import logging
import os

APKTOOL_PATH = r'util/apktool/apktool.jar'
AXML_EDITOR_PATH = r'util/AXMLUtil/AXMLEditor.jar'
AXML_PRINTER_PATH = r'util/AXMLUtil/AXMLPrinter3.jar'

KEY_STORE_PATH = r'util/apk/dll_sign.jks'
KEY_STORE_PWD = r'123456'
KEY_STORE_ALIAS = r'dll'
KEY_STORE_ALIAS_PWD = r'123456'

APK_IN = r'apk/in'
APK_OUT = r'apk/out'
APK_SHELL = r'apk/shell'
APK_TMP_IN = r'tmp/in'
APK_TMP_OUT = r'tmp/out'
APK_TMP_SHELL = r'tmp/shell'
LOG_PATH = r'log'

if not os.path.exists(APK_IN):
    os.makedirs(APK_IN)

if not os.path.exists(APK_OUT):
    os.mkdir(APK_OUT)

if not os.path.exists(APK_SHELL):
    os.mkdir(APK_SHELL)

if not os.path.exists(APK_TMP_IN):
    os.makedirs(APK_TMP_IN)

if not os.path.exists(APK_TMP_OUT):
    os.mkdir(APK_TMP_OUT)

if not os.path.exists(APK_TMP_SHELL):
    os.mkdir(APK_TMP_SHELL)

if not os.path.exists(LOG_PATH):
    os.mkdir(LOG_PATH)

APKTOOL_PATH = os.path.abspath(APKTOOL_PATH)
AXML_EDITOR_PATH = os.path.abspath(AXML_EDITOR_PATH)
AXML_PRINTER_PATH = os.path.abspath(AXML_PRINTER_PATH)
KEY_STORE_PATH = os.path.abspath(KEY_STORE_PATH)

APK_IN = os.path.abspath(APK_IN)
APK_OUT = os.path.abspath(APK_OUT)
APK_SHELL = os.path.abspath(APK_SHELL)
APK_TMP_IN = os.path.abspath(APK_TMP_IN)
APK_TMP_OUT = os.path.abspath(APK_TMP_OUT)
APK_TMP_SHELL = os.path.abspath(APK_TMP_SHELL)
LOG_PATH = os.path.abspath(LOG_PATH)

logging.basicConfig(level=logging.DEBUG,
                    format='%(levelname)s: %(asctime)s %(filename)s[line:%(lineno)4d]-%(name)s.%(funcName)s: '
                           '%(message)s',
                    datefmt='%d %b %Y %H:%M:%S',
                    handlers=[
                        logging.StreamHandler(),
                        logging.FileHandler(filename=LOG_PATH + '/' + 'log.log', encoding='utf-8', mode='w')],
                    )
