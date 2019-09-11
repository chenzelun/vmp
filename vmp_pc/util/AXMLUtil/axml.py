import logging
from subprocess import Popen, PIPE
from typing import Tuple

import chardet


class AndroidXML:
    logger = logging.getLogger(__name__)

    @staticmethod
    def insert_attr(tag, name, key, value, in_path, out_path, editor_path, log=True):
        child = Popen(
            ['java', '-jar', editor_path, '-attr', '-i', tag, name, key, value, in_path, out_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @staticmethod
    def remove_attr(tag, name, key, in_path, out_path, editor_path, log=True):
        child = Popen(
            ['java', '-jar', editor_path, '-attr', '-r', tag, name, key, in_path, out_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @staticmethod
    def modify_attr(tag, name, key, value, in_path, out_path, editor_path, log=True):
        child = Popen(
            ['java', '-jar', editor_path, '-attr', '-m', tag, name, key, value, in_path, out_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @staticmethod
    def insert_tag(tag_xml_path, in_path, out_path, editor_path, log=True):
        child = Popen(
            ['java', '-jar', editor_path, '-tag', '-i', tag_xml_path, in_path, out_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @staticmethod
    def remove_tag(tag, name, in_path, out_path, editor_path, log=True):
        child = Popen(
            ['java', '-jar', editor_path, '-tag', '-r', tag, name, in_path, out_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @staticmethod
    def print_xml(in_path, printer_path, log=True):
        child = Popen(
            ['java', '-jar', printer_path, in_path],
            stdout=PIPE, stderr=PIPE)
        AndroidXML.__log(child, log)

    @classmethod
    def axml2xml(cls, in_path, printer_path, log=True) -> Tuple[bytes, bytes]:
        child = Popen(
            ['java', '-jar', printer_path, in_path],
            stdout=PIPE, stderr=PIPE)
        return AndroidXML.__log(child, log)

    @classmethod
    def __log(cls, child, log):
        out_str, err_str = child.communicate()
        if log:
            if out_str:
                cls.logger.debug(str(out_str, encoding=chardet.detect(out_str)['encoding']))
            if err_str:
                cls.logger.error(str(err_str, encoding=chardet.detect(err_str)['encoding']))
        return out_str, err_str
