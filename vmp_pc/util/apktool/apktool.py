import logging
from subprocess import Popen, PIPE

import chardet


class ApkTool:
    logger = logging.getLogger(__name__)

    @staticmethod
    def decode(in_path: str, out_path: str, apk_tool_path: str, log=True):
        child = Popen(
            ['java', '-jar', '-Duser.language=en', '-Dfile.encoding=UTF8', apk_tool_path,
             'd', '-f', '-o', out_path, in_path], stdout=PIPE, stderr=PIPE)
        ApkTool.__log(child, log)

    @staticmethod
    def build(in_path: str, out_path: str, apk_tool_path: str, log=True):
        child = Popen(
            ['java', '-jar', '-Duser.language=en', '-Dfile.encoding=UTF8', apk_tool_path,
             'b', '-f', '-o', out_path, in_path], stdout=PIPE, stderr=PIPE)
        ApkTool.__log(child, log)

    @classmethod
    def __log(cls, child, log):
        out_str, err_str = child.communicate()
        if log:
            if out_str:
                cls.logger.debug(str(out_str, encoding=chardet.detect(out_str)['encoding']))
            if err_str:
                cls.logger.error(str(err_str, encoding=chardet.detect(err_str)['encoding']))
        return out_str, err_str
