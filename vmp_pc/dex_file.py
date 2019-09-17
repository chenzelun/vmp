import logging
from abc import ABCMeta, abstractmethod
from datetime import date
from enum import IntEnum, unique
from hashlib import sha1
from struct import unpack_from, pack
from typing import List, Dict, Union, Tuple, Set
from zlib import adler32


@unique
class MapListItemType(IntEnum):
    TYPE_HEADER_ITEM = 0x0000
    TYPE_STRING_ID_ITEM = 0x0001
    TYPE_TYPE_ID_ITEM = 0x0002
    TYPE_PROTO_ID_ITEM = 0x0003
    TYPE_FIELD_ID_ITEM = 0x0004
    TYPE_METHOD_ID_ITEM = 0x0005
    TYPE_CLASS_DEF_ITEM = 0x0006
    TYPE_MAP_LIST = 0x1000
    TYPE_TYPE_LIST = 0x1001
    TYPE_ANNOTATION_SET_REF_LIST = 0x1002
    TYPE_ANNOTATION_SET_ITEM = 0x1003
    TYPE_CLASS_DATA_ITEM = 0x2000
    TYPE_CODE_ITEM = 0x2001
    TYPE_STRING_DATA_ITEM = 0x2002
    TYPE_DEBUG_INFO_ITEM = 0x2003
    TYPE_ANNOTATION_ITEM = 0x2004
    TYPE_ENCODED_ARRAY_ITEM = 0x2005
    TYPE_ANNOTATIONS_DIRECTORY_ITEM = 0x2006


@unique
class EndianTag(IntEnum):
    ENDIAN_CONSTANT = 0x12345678
    REVERSE_ENDIAN_CONSTANT = 0x78563412


@unique
class EncodedValueType(IntEnum):
    VALUE_BYTE = 0x00
    VALUE_SHORT = 0x02
    VALUE_CHAR = 0x03
    VALUE_INT = 0x04
    VALUE_LONG = 0x06
    VALUE_FLOAT = 0x10
    VALUE_DOUBLE = 0x11
    VALUE_STRING = 0x17
    VALUE_TYPE = 0x18
    VALUE_FIELD = 0x19
    VALUE_METHOD = 0x1a
    VALUE_ENUM = 0x1b
    VALUE_ARRAY = 0x1c
    VALUE_ANNOTATION = 0x1d
    VALUE_NULL = 0x1e
    VALUE_BOOLEAN = 0x1f


class AccessFlag(IntEnum):
    ACC_PUBLIC = 0x00000001  # class , field, method, ic
    ACC_PRIVATE = 0x00000002  # field, method, ic
    ACC_PROTECTED = 0x00000004  # field, method, ic
    ACC_STATIC = 0x00000008  # field, method, ic
    ACC_FINAL = 0x00000010  # class , field, method, ic
    ACC_SYNCHRONIZED = 0x00000020  # method(only allowed on natives)
    ACC_SUPER = 0x00000020  # class(not used in Dalvik)
    ACC_VOLATILE = 0x00000040  # field
    ACC_BRIDGE = 0x00000040  # method(1.5)
    ACC_TRANSIENT = 0x00000080  # field
    ACC_VARARGS = 0x00000080  # method(1.5)
    ACC_NATIVE = 0x00000100  # method
    ACC_INTERFACE = 0x00000200  # class , ic
    ACC_ABSTRACT = 0x00000400  # class , method, ic
    ACC_STRICT = 0x00000800  # method
    ACC_SYNTHETIC = 0x00001000  # field, method, ic
    ACC_ANNOTATION = 0x00002000  # class , ic (1.5)
    ACC_ENUM = 0x00004000  # class , field, ic (1.5)
    ACC_CONSTRUCTOR = 0x00010000  # method(Dalvik only)
    ACC_DECLARED_SYNCHRONIZED = 0x00020000  # method(Dalvik only)


@unique
class DebugInfoOpCodes(IntEnum):
    DBG_END_SEQUENCE = 0x00
    DBG_ADVANCE_PC = 0x01
    DBG_ADVANCE_LINE = 0x02
    DBG_START_LOCAL = 0x03
    DBG_START_LOCAL_EXTENDED = 0x04
    DBG_END_LOCAL = 0x05
    DBG_RESTART_LOCAL = 0x06
    DBG_SET_PROLOGUE_END = 0x07
    DBG_SET_EPILOGUE_BEGIN = 0x08
    DBG_SET_FILE = 0x09
    DBG_FIRST_SPECIAL = 0x0a
    DBG_LINE_BASE = -4
    DBG_LINE_RANGE = 15


class Index:
    def __init__(self, index: int):
        self.index: int = index


class Leb128:
    @staticmethod
    def read_unsigned_leb128(buf: bytes, offset: Index) -> int:
        result: int = buf[offset.index]
        offset.index += 1
        if result > 0x7f:
            cur: int = buf[offset.index]
            offset.index += 1
            result = (result & 0x7f) | ((cur & 0x7f) << 7)
            if cur > 0x7f:
                cur = buf[offset.index]
                offset.index += 1
                result |= (cur & 0x7f) << 14
                if cur > 0x7f:
                    cur = buf[offset.index]
                    offset.index += 1
                    result |= (cur & 0x7f) << 21
                    if cur > 0x7f:
                        cur = buf[offset.index]
                        offset.index += 1
                        result |= cur << 28

        return result

    @staticmethod
    def write_unsigned_leb128(data: int) -> bytearray:
        result: bytearray = bytearray()
        while True:
            out: int = data & 0x7f
            if out != data:
                result.append(out | 0x80)
                data >>= 7
            else:
                result.append(out)
                break
        return result

    @staticmethod
    def pass_leb128(buf: bytes, offset: Index, size: int = 1):
        offset.offset = 1
        while size > 0:
            if buf[offset.index] <= 0x7f:
                size -= 1
            offset.index += 1

    @staticmethod
    def read_signed_leb128(buf: bytes, offset: Index) -> int:
        result: int = buf[offset.index]
        offset.index += 1
        if result <= 0x7f:
            if result > 0x3f:
                result |= ~0x7f
        else:
            cur = buf[offset.index]
            offset.index += 1
            result = (result & 0x7f) | ((cur & 0x7f) << 7)
            if cur <= 0x7f:
                if cur > 0x3f:
                    result |= ~0x3fff
            else:
                cur = buf[offset.index]
                offset.index += 1
                result |= (cur & 0x7f) << 14
                if cur <= 0x7f:
                    if cur > 0x3f:
                        result |= ~0x1fffff
                else:
                    cur = buf[offset.index]
                    offset.index += 1
                    result |= (cur & 0x7f) << 21
                    if cur <= 0x7f:
                        if cur > 0x3f:
                            result |= ~0x0fffffff
                    else:
                        cur = buf[offset.index]
                        offset.index += 1
                        result |= cur << 28
                        if cur > 0x07:
                            result |= ~0x7fffffff
        return result

    @staticmethod
    def write_signed_leb128(data: int) -> bytearray:
        result: bytearray = bytearray()
        while True:
            out: int = data & 0x7f
            data >>= 7
            if data == 0x00 or (data == ~0x00 and out > 0x3f):
                result.append(out)
                break
            else:
                result.append(out | 0x80)
        return result

    @staticmethod
    def read_unsigned_leb128pl(buf: bytes, offset: Index) -> int:
        return Leb128.read_unsigned_leb128(buf, offset) - 1

    @staticmethod
    def write_unsigned_leb128pl(data: int) -> bytearray:
        return Leb128.write_unsigned_leb128(data + 1)


class ABCItem(metaclass=ABCMeta):
    def __init__(self):
        self.dex_offset = 0

    @abstractmethod
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        """
        bytes --> struct data
        :param buf: dex buf
        :param offset: offset in dex file
        :param kwargs:
        :return: self
        """
        pass

    @abstractmethod
    def to_bytes(self, offset: Index = None) -> bytes:
        """
        struct data --> bytes
        :return: bytes
        """
        pass

    @staticmethod
    def aligned(buf: bytearray, offset: Index, aligned: int):
        size = aligned - (offset.index & (aligned - 1))
        if size == aligned:
            return
        buf.extend(b'\x00' * size)
        offset.index += size


class EncodedValue(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        first_byte = buf[offset.index]
        offset.index += 0x01
        self.value_arg = (first_byte >> 5) & 0x07
        self.value_type = first_byte & 0x1f
        if self.value_type == EncodedValueType.VALUE_ARRAY.value:
            self.value = EncodedArrayItem.pass_item(buf, offset)
        elif self.value_type == EncodedValueType.VALUE_ANNOTATION.value:
            self.value = EncodedAnnotation.pass_item(buf, offset)
        elif self.value_type == EncodedValueType.VALUE_NULL.value or \
                self.value_type == EncodedValueType.VALUE_BOOLEAN.value:
            pass
        else:
            self.value = buf[offset.index: offset.index + self.value_arg + 1]
            offset.index += self.value_arg + 1
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        r = bytearray(((self.value_arg << 5) | self.value_type).to_bytes(1, byteorder='little', signed=False))
        if self.value_type == EncodedValueType.VALUE_NULL.value or \
                self.value_type == EncodedValueType.VALUE_BOOLEAN.value:
            pass
        else:
            r.extend(self.value)
        return bytes(r)

    @staticmethod
    def pass_value(buf: bytes, offset: Index, n: int = 1) -> bytes:
        base_offset = offset.index
        for _ in range(n):
            first_byte = buf[offset.index]
            offset.index += 0x01
            value_arg = (first_byte >> 5) & 0x07
            value_type = first_byte & 0x1f

            if value_type == EncodedValueType.VALUE_ARRAY.value:
                EncodedArrayItem.pass_item(buf, offset)
            elif value_type == EncodedValueType.VALUE_ANNOTATION.value:
                EncodedAnnotation.pass_item(buf, offset)
            elif value_type == EncodedValueType.VALUE_NULL.value or \
                    value_type == EncodedValueType.VALUE_BOOLEAN.value:
                pass
            else:
                offset.index += value_arg + 1
        return buf[base_offset:offset.index]

    def __init__(self):
        super().__init__()
        self.value_arg: int = 0
        self.value_type: int = 0
        self.value: bytes = b''


class AnnotationElement(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.name_idx = Leb128.read_unsigned_leb128(buf, offset)
        self.value = EncodedValue().parse(buf, offset)
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return Leb128.write_unsigned_leb128(self.name_idx) + self.value.to_bytes()

    @staticmethod
    def pass_element(buf: bytes, offset: Index, n: int = 1) -> bytes:
        base_offset = offset.index
        for _ in range(n):
            Leb128.pass_leb128(buf, offset)
            EncodedValue.pass_value(buf, offset)
        return buf[base_offset:offset.index]

    def __init__(self):
        super().__init__()
        self.name_idx: int = 0
        self.value: Union[EncodedValue, None] = None


class EncodedAnnotation(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.type_idx = Leb128.read_unsigned_leb128(buf, offset)
        self.size = Leb128.read_unsigned_leb128(buf, offset)
        for _ in range(self.size):
            self.elements.append(AnnotationElement().parse(buf, offset))
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        r = bytearray(Leb128.write_unsigned_leb128(self.type_idx))
        r.extend(Leb128.write_unsigned_leb128(self.size))
        for element in self.elements:
            r.extend(element.to_bytes())
        return bytes(r)

    @staticmethod
    def pass_item(buf: bytes, offset: Index, n: int = 1) -> bytes:
        base_offset = offset.index
        for _ in range(n):
            Leb128.pass_leb128(buf, offset)
            size = Leb128.read_unsigned_leb128(buf, offset)
            AnnotationElement.pass_element(buf, offset, size)
        return buf[base_offset:offset.index]

    def __init__(self):
        super().__init__()
        self.type_idx: int = 0
        self.size: int = 0
        self.elements: List[AnnotationElement] = []


class EncodedField(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.field_idx_diff = Leb128.read_unsigned_leb128(buf, offset)
        self.access_flags = Leb128.read_unsigned_leb128(buf, offset)
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return Leb128.write_unsigned_leb128(self.field_idx_diff) + Leb128.write_unsigned_leb128(self.access_flags)

    def __init__(self):
        super().__init__()
        self.field_idx_diff: int = 0
        self.access_flags: int = 0


class TryItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.start_addr, self.insns_count, self.handler_off = unpack_from('<I2H', buf, offset.index)
        offset.index += 0x08
        return self

    @staticmethod
    def pass_item(buf: bytes, offset: Index, n: int = 1):
        while n >= 0:
            n -= 1

            offset.index += 0x08

        return buf[offset.index - 0x08:offset.index]

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<I2H', self.start_addr, self.insns_count, self.handler_off)

    def __init__(self):
        super().__init__()
        self.start_addr: int = 0
        self.insns_count: int = 0
        self.handler_off: int = 0


class EncodedTypeAddrPair(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.type_idx = Leb128.read_unsigned_leb128(buf, offset)
        self.addr = Leb128.read_unsigned_leb128(buf, offset)
        return self

    @staticmethod
    def pass_pair(buf: bytes, offset: Index, n: int = 1):
        start_index = offset.index
        Leb128.pass_leb128(buf, offset, 2 * n)
        return buf[start_index:offset.index]

    def to_bytes(self, offset: Index = None) -> bytes:
        return Leb128.write_unsigned_leb128(self.type_idx) + Leb128.write_unsigned_leb128(self.addr)

    def __init__(self):
        super().__init__()
        self.type_idx: int = 0
        self.addr: int = 0


class EncodedCatchHandler(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.size = Leb128.read_signed_leb128(buf, offset)
        self.handlers: List[EncodedTypeAddrPair] = []
        for _ in range(abs(self.size)):
            self.handlers.append(EncodedTypeAddrPair().parse(buf, offset))

        if self.size <= 0:
            self.catch_all_addr = Leb128.read_unsigned_leb128(buf, offset)
        return self

    @staticmethod
    def pass_handler(buf: bytes, offset: Index, n: int = 1):
        start_index = offset.index
        while n > 0:
            n -= 1

            size = Leb128.read_signed_leb128(buf, offset)
            EncodedTypeAddrPair.pass_pair(buf, offset, abs(size))
            if size <= 0:
                Leb128.pass_leb128(buf, offset)

        return buf[start_index:offset.index]

    def to_bytes(self, offset: Index = None) -> bytes:
        r = bytearray(Leb128.write_signed_leb128(self.size))
        for handler in self.handlers:
            r.extend(handler.to_bytes())

        if self.size <= 0:
            r.extend(Leb128.write_unsigned_leb128(self.catch_all_addr))
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.size: int = 0
        self.handlers: List[EncodedTypeAddrPair] = []
        self.catch_all_addr: int = 0


class EncodedCatchHandlerList(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        base_index = offset.index
        self.size = Leb128.read_unsigned_leb128(buf, offset)
        self.list: Dict[int, EncodedCatchHandler] = {}
        for _ in range(self.size):
            cur_index = offset.index
            self.list[cur_index - base_index] = EncodedCatchHandler().parse(buf, offset)
        return self

    @staticmethod
    def pass_list(buf: bytes, offset: Index, n: int = 1):
        start_index = offset.index
        while n > 0:
            n -= 1

            size = Leb128.read_unsigned_leb128(buf, offset)
            EncodedCatchHandler.pass_handler(buf, offset, size)

        return buf[start_index:offset.index]

    def to_bytes(self, offset: Index = None) -> bytes:
        r = bytearray(Leb128.write_unsigned_leb128(self.size))
        for key in sorted(self.list.keys()):
            r.extend(self.list[key].to_bytes())
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.size: int = 0
        self.list: Dict[int, EncodedCatchHandler] = {}


class MapListItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.type, \
        self.unused, \
        self.size, \
        self.offset = unpack_from('<2H2I', buf, offset.index)
        offset.index += 0x0c
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2H2I', self.type, self.unused, self.size, self.offset)

    def __init__(self, item_type: int = 0, unused: int = 0, size: int = 0, offset: int = 0):
        super().__init__()
        self.type = item_type
        self.unused = unused
        self.size = size
        self.offset = offset


class ListPool:
    def __init__(self, item_type: type):
        self.pool: List[item_type] = []
        self.item_type = item_type
        self.map_list_item: Union[MapListItem, None] = None

    def parse(self, buf: bytes, map_list_item: MapListItem, **kwargs):
        self.map_list_item = map_list_item
        offset = Index(self.map_list_item.offset)
        for _ in range(map_list_item.size):
            self.pool.append(self.item_type().parse(buf, offset, kwargs))
        return self

    def __getitem__(self, item):
        return self.pool[item]

    def __setitem__(self, key, value):
        self.pool[key] = value

    def __len__(self):
        return len(self.pool)

    def write(self, buf: bytearray, offset: Index, **kwargs):
        if 'aligned' in kwargs:
            self.aligned(buf, offset, kwargs['aligned'])

        self.map_list_item.offset = offset.index
        self.map_list_item.size = len(self.pool)
        offset.index -= len(buf)
        for item in self.pool:
            buf.extend(item.to_bytes())
        offset.index += len(buf)
        return self.map_list_item

    @staticmethod
    def aligned(buf: bytearray, offset: Index, aligned: int):
        size = aligned - (offset.index & (aligned - 1))
        if size == aligned:
            return
        buf.extend(b'\x00' * size)
        offset.index += size


class DictPool:
    def __init__(self, item_type: type):
        self.pool: Dict[int, item_type] = {}
        self.item_type = item_type
        self.map_list_item: Union[MapListItem, None] = None

    def parse(self, buf: bytes, map_list_item: MapListItem, **kwargs):
        self.map_list_item = map_list_item
        offset = Index(self.map_list_item.offset)
        for _ in range(map_list_item.size):
            cur_offset = offset.index
            self.pool[cur_offset] = self.item_type().parse(buf, offset, kwargs)
        return self

    def __getitem__(self, item):
        return self.pool[item] if item != 0 else None

    def __setitem__(self, key, value):
        self.pool[key] = value

    def __len__(self):
        return len(self.pool)

    def write(self, buf: bytearray, offset: Index, **kwargs):
        if 'aligned' in kwargs:
            self.aligned(buf, offset, kwargs['aligned'])

        self.map_list_item.offset = offset.index
        self.map_list_item.size = len(self.pool)
        for item in self.pool.values():
            buf.extend(item.to_bytes(offset))
        return self.map_list_item

    @staticmethod
    def aligned(buf: bytearray, offset: Index, aligned: int):
        size = aligned - (offset.index & (aligned - 1))
        if size == aligned:
            return
        buf.extend(b'\x00' * size)
        offset.index += size


class MapList(DictPool):
    def __init__(self):
        super().__init__(MapListItem)

    def parse(self, buf: bytes, map_off: int, **kwargs):
        offset = Index(map_off)
        size = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04
        for _ in range(size):
            map_list_item = MapListItem().parse(buf, offset, kwargs)
            self.pool[map_list_item.type] = map_list_item

        self.map_list_item = self.pool[MapListItemType.TYPE_MAP_LIST]
        return self

    def __getitem__(self, item: MapListItemType):
        return self.pool[item.value]

    def __setitem__(self, key: MapListItemType, value: MapListItem):
        self.pool[key.value] = value

    def write(self, buf: bytearray, offset: Index, **kwargs):
        self.aligned(buf, offset, 4)
        self.map_list_item.offset = offset.index
        self.map_list_item.size = 1
        offset.index -= len(buf)
        buf.extend(pack('<I', len(self.pool)))
        for item in sorted(self.pool.values(), key=lambda x: x.offset):
            buf.extend(item.to_bytes())
        offset.index += len(buf)
        return self.map_list_item


class EncodedArrayItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        self.size = Leb128.read_unsigned_leb128(buf, offset)
        for _ in range(self.size):
            self.values.append(EncodedValue().parse(buf, offset))
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(Leb128.write_unsigned_leb128(self.size))
        for v in self.values:
            r.extend(v.to_bytes())
        offset.index += len(r)
        return bytes(r)

    @staticmethod
    def pass_item(buf: bytes, offset: Index, n: int = 1) -> bytes:
        base_offset = offset.index
        for _ in range(n):
            size = Leb128.read_unsigned_leb128(buf, offset)
            EncodedValue.pass_value(buf, offset, size)
        return buf[base_offset:offset.index]

    def __init__(self):
        super().__init__()
        self.dex_offset: int = 0
        self.size: int = 0
        self.values: List[EncodedValue] = []


class EncodedArray(DictPool):
    def __init__(self):
        super().__init__(EncodedArrayItem)


class StringIdItem(ABCItem):
    def __init__(self):
        super().__init__()
        self.string_data: bytes = b''

    def parse(self, buf: bytes, offset: Index, **kwargs):
        self.dex_offset = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04

        data_index = Index(self.dex_offset)
        Leb128.pass_leb128(buf, data_index)
        end_index = buf.find(0, data_index.index) + 1
        self.string_data = buf[self.dex_offset:end_index]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        offset.index += len(self.string_data)
        return self.string_data


class StringId(ListPool):
    def __init__(self):
        super().__init__(StringIdItem)
        self.map_list_item_string_data: Union[MapListItem, None] = None

    def parse(self, buf: bytes, map_list_item: MapListItem, map_list_item_string_data: MapListItem = None):
        """

        :param buf:
        :param map_list_item: string_idx's
        :param map_list_item_string_data: string_data's, must not None
        """
        if map_list_item_string_data:
            self.map_list_item_string_data = map_list_item_string_data
        else:
            raise RuntimeError('map_list_item_string_data is None')

        self.map_list_item = map_list_item
        offset = Index(self.map_list_item.offset)
        for _ in range(self.map_list_item.size):
            self.pool.append(StringIdItem().parse(buf, offset))
        return self

    def write(self, buf: bytearray, offset: Index, data_buf: bytearray = None, data_offset: Index = None):
        """

        :param buf: string_idx's
        :param offset: string_idx's
        :param data_buf: string_data's, must not None
        :param data_offset: string_data's, must not None
        :return:
        """
        if data_buf is None or data_offset is None:
            raise RuntimeError('data_buf is None or data_offset is None')

        self.aligned(buf, offset, 4)
        self.map_list_item.offset = offset.index
        self.map_list_item_string_data.offset = data_offset.index
        offset.index -= len(buf)
        for item in self.pool:
            data_buf.extend(item.to_bytes(data_offset))
            buf.extend(pack('<I', item.dex_offset))
        offset.index += len(buf)
        return self.map_list_item, self.map_list_item_string_data


class TypeIdItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.descriptor_idx = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<I', self.descriptor_idx)

    def __init__(self):
        super().__init__()
        self.descriptor_idx: int = 0


class TypeId(ListPool):
    def __init__(self):
        super().__init__(TypeIdItem)


class TypeListItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        self.size = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04
        if self.size:
            self.list = list(unpack_from('<' + str(self.size) + 'H', buf, offset.index))
            offset.index += self.size * 0x02

        if offset.index & 0x03:
            offset.index = (offset.index | 0x03) + 0x01
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray()
        r.extend(pack('<I', self.size))
        for idx in self.list:
            r.extend(pack('<H', idx))

        offset.index += len(r)
        self.aligned(r, offset, 4)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.size: int = 0
        self.list: List[int] = []


class TypeList(DictPool):
    def __init__(self):
        super().__init__(TypeListItem)


class ProtoIdItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        type_list: TypeList = kwargs['type_list']
        self.shorty_idx, self.return_type_idx, parameters_off = unpack_from('<3I', buf, offset.index)
        self.parameters: TypeListItem = type_list[parameters_off]
        offset.index += 0x0c
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<3I', self.shorty_idx, self.return_type_idx, self.parameters.dex_offset if self.parameters else 0)

    def __init__(self):
        super().__init__()
        self.shorty_idx: int = 0
        self.return_type_idx: int = 0
        self.parameters: Union[TypeListItem, None] = None


class ProtoId(ListPool):
    def __init__(self):
        super().__init__(ProtoIdItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, type_list: TypeList = None):
        """

        :param buf:
        :param map_list_item:
        :param type_list: must not None
        :return:
        """
        if type_list is None:
            raise RuntimeError('type_list is None')
        super().parse(buf, map_list_item, type_list=type_list)
        return self


class FieldIdItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.class_idx, self.type_idx, self.name_idx = unpack_from('<2HI', buf, offset.index)
        offset.index += 0x08
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2HI', self.class_idx, self.type_idx, self.name_idx)

    def __init__(self):
        super().__init__()
        self.class_idx: int = 0
        self.type_idx: int = 0
        self.name_idx: int = 0


class FieldId(ListPool):
    def __init__(self):
        super().__init__(FieldIdItem)


class MethodIdItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.class_idx, self.proto_idx, self.name_idx = unpack_from('<2HI', buf, offset.index)
        offset.index += 0x08
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2HI', self.class_idx, self.proto_idx, self.name_idx)

    def __init__(self):
        super().__init__()
        self.class_idx: int = 0
        self.proto_idx: int = 0
        self.name_idx: int = 0


class MethodId(ListPool):
    def __init__(self):
        super().__init__(MethodIdItem)


class DebugInfoItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        self.line_start = Leb128.read_unsigned_leb128(buf, offset)
        self.parameters_size = Leb128.read_unsigned_leb128(buf, offset)
        for _ in range(self.parameters_size):
            self.parameters_name.append(Leb128.read_unsigned_leb128pl(buf, offset))

        start_index = offset.index
        while True:
            op_code = buf[offset.index]
            offset.index += 0x01

            if op_code == DebugInfoOpCodes.DBG_END_SEQUENCE.value:
                break
            elif op_code in (DebugInfoOpCodes.DBG_ADVANCE_LINE.value,
                             DebugInfoOpCodes.DBG_ADVANCE_PC.value,
                             DebugInfoOpCodes.DBG_END_LOCAL.value,
                             DebugInfoOpCodes.DBG_RESTART_LOCAL.value,
                             DebugInfoOpCodes.DBG_SET_FILE.value):
                Leb128.pass_leb128(buf, offset)
            elif op_code == DebugInfoOpCodes.DBG_START_LOCAL.value:
                Leb128.pass_leb128(buf, offset, 3)
            elif op_code == DebugInfoOpCodes.DBG_START_LOCAL_EXTENDED.value:
                Leb128.pass_leb128(buf, offset, 4)
            # elif op_code == DebugInfoOpCodes.DBG_SET_EPILOGUE_BEGIN.value or \
            #         op_code == DebugInfoOpCodes.DBG_SET_PROLOGUE_END.value:
            #     pass

        self.debug_info = buf[start_index:offset.index]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(Leb128.write_unsigned_leb128(self.line_start))
        r.extend(Leb128.write_unsigned_leb128(self.parameters_size))
        for idx in self.parameters_name:
            r.extend(Leb128.write_unsigned_leb128pl(idx))

        r.extend(self.debug_info)
        offset.index += len(r)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.line_start: int = 0
        self.parameters_size: int = 0
        self.parameters_name: List[int] = []
        self.debug_info: bytes = b''


class DebugInfo(DictPool):
    def __init__(self):
        super().__init__(DebugInfoItem)


class CodeItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        debug_info: DebugInfo = kwargs['debug_info']
        self.dex_offset = offset.index
        self.registers_size, \
        self.ins_size, \
        self.outs_size, \
        self.tries_size, \
        debug_info_off, \
        self.insns_size = unpack_from('<4H2I', buf, offset.index)
        offset.index += 0x10

        if self.insns_size:
            self.insns: bytes = buf[offset.index:offset.index + self.insns_size * 2]
            offset.index += self.insns_size * 2

        if self.tries_size and (self.insns_size & 0x01):
            offset.index += 2

        if self.tries_size:
            self.tries: List[TryItem] = []
            for _ in range(self.tries_size):
                self.tries.append(TryItem().parse(buf, offset))

            self.handlers = EncodedCatchHandlerList().parse(buf, offset)

        self.debug_info = debug_info[debug_info_off]
        if offset.index & 0x03:
            offset.index = (offset.index | 0x03) + 0x01
        return self

    @staticmethod
    def pass_item(buf: bytes, offset: Index, n: int = 1):
        start_index = offset.index

        while n > 0:
            n -= 1

            registers_size, \
            ins_size, \
            outs_size, \
            tries_size, \
            debug_info_off, \
            insns_size = unpack_from('<4H2I', buf, offset.index)
            offset.index += 0x10

            if insns_size:
                offset.index += insns_size * 2

            if tries_size and (insns_size & 0x01):
                offset.index += 2

            if tries_size:
                TryItem.pass_item(buf, offset, tries_size)
                EncodedCatchHandlerList.pass_list(buf, offset)

            if offset.index & 0x03:
                offset.index = (offset.index | 0x03) + 0x01

        return buf[start_index: offset.index]

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(pack('<4H2I',
                           self.registers_size,
                           self.ins_size,
                           self.outs_size,
                           self.tries_size,
                           self.debug_info.dex_offset if self.debug_info else 0,
                           self.insns_size))

        if self.insns_size:
            r.extend(self.insns)

        if self.tries_size and (self.insns_size & 0x01):
            r.extend([0x00, 0x00])

        if self.tries_size:
            for item in self.tries:
                r.extend(item.to_bytes())
            r.extend(self.handlers.to_bytes())

        offset.index += len(r)
        self.aligned(r, offset, 4)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.registers_size: int = 0
        self.ins_size: int = 0
        self.outs_size: int = 0
        self.tries_size: int = 0
        self.debug_info: Union[DebugInfoItem, None] = None
        self.insns_size: int = 0
        self.insns: bytes = b''
        self.tries: List[TryItem] = []
        self.handlers: Union[EncodedCatchHandlerList, None] = None


class Code(DictPool):
    def __init__(self):
        super().__init__(CodeItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, debug_info: DebugInfo = None):
        """

        :param buf:
        :param map_list_item:
        :param debug_info: must not None
        :return:
        """
        if debug_info is None:
            raise RuntimeError('debug_info is None')
        super().parse(buf, map_list_item, debug_info=debug_info)
        return self


class EncodedMethod(ABCItem):
    def parse(self, buf: bytes, offset: Index, code: Code = None):
        """

        :param buf:
        :param offset:
        :param code: must not None
        :return:
        """
        if code is None:
            raise RuntimeError('code is None')
        self.method_idx_diff = Leb128.read_unsigned_leb128(buf, offset)
        self.access_flags = Leb128.read_unsigned_leb128(buf, offset)
        code_off = Leb128.read_unsigned_leb128(buf, offset)
        self.code = code[code_off]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return Leb128.write_unsigned_leb128(self.method_idx_diff) + \
               Leb128.write_unsigned_leb128(self.access_flags) + \
               Leb128.write_unsigned_leb128(self.code.dex_offset if self.code else 0)

    def __init__(self):
        super().__init__()
        self.method_idx_diff: int = 0
        self.access_flags: int = 0
        self.code: Union[CodeItem, None] = None


class ClassDataItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        code: Code = kwargs['code']

        self.static_fields_size = Leb128.read_unsigned_leb128(buf, offset)
        self.instance_fields_size = Leb128.read_unsigned_leb128(buf, offset)
        self.direct_methods_size = Leb128.read_unsigned_leb128(buf, offset)
        self.virtual_methods_size = Leb128.read_unsigned_leb128(buf, offset)
        self.static_fields: List[EncodedField] = []
        for _ in range(self.static_fields_size):
            self.static_fields.append(EncodedField().parse(buf, offset))

        self.instance_fields: List[EncodedField] = []
        for _ in range(self.instance_fields_size):
            self.instance_fields.append(EncodedField().parse(buf, offset))

        self.direct_methods: List[EncodedMethod] = []
        for _ in range(self.direct_methods_size):
            self.direct_methods.append(EncodedMethod().parse(buf, offset, code))

        self.virtual_methods: List[EncodedMethod] = []
        for _ in range(self.virtual_methods_size):
            self.virtual_methods.append(EncodedMethod().parse(buf, offset, code))
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(Leb128.write_unsigned_leb128(self.static_fields_size))
        r.extend(Leb128.write_unsigned_leb128(self.instance_fields_size))
        r.extend(Leb128.write_unsigned_leb128(self.direct_methods_size))
        r.extend(Leb128.write_unsigned_leb128(self.virtual_methods_size))

        for field in self.static_fields:
            r.extend(field.to_bytes())
        for field in self.instance_fields:
            r.extend(field.to_bytes())
        for method in self.direct_methods:
            r.extend(method.to_bytes())
        for method in self.virtual_methods:
            r.extend(method.to_bytes())

        offset.index += len(r)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.static_fields_size: int = 0
        self.instance_fields_size: int = 0
        self.direct_methods_size: int = 0
        self.virtual_methods_size: int = 0
        self.static_fields: List[EncodedField] = []
        self.instance_fields: List[EncodedField] = []
        self.direct_methods: List[EncodedMethod] = []
        self.virtual_methods: List[EncodedMethod] = []


class ClassData(DictPool):
    def __init__(self):
        super().__init__(ClassDataItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, code: Code = None):
        """

        :param buf:
        :param map_list_item:
        :param code: must not None
        :return:
        """
        if code is None:
            raise RuntimeError('code is None')
        super().parse(buf, map_list_item, code=code)
        return self


class AnnotationItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        self.visibility = buf[offset.index]
        offset.index += 0x01
        self.annotation = EncodedAnnotation().parse(buf, offset)
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(self.visibility.to_bytes(1, byteorder='little', signed=False))
        r.extend(self.annotation.to_bytes())
        offset.index += len(r)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.visibility: int = 0
        self.annotation: Union[EncodedAnnotation, None] = None


class Annotation(DictPool):
    def __init__(self):
        super().__init__(AnnotationItem)


class AnnotationSetItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        annotation: Annotation = kwargs['annotation']

        self.size = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04
        if self.size:
            self.entries: List[AnnotationItem] = []
            for annotation_off in unpack_from('<' + str(self.size) + 'I', buf, offset.index):
                self.entries.append(annotation[annotation_off])
            offset.index += self.size * 0x04
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(pack('<I', self.size))
        if self.size:
            for item in self.entries:
                r.extend(pack('<I', item.dex_offset))

        offset.index += len(r)
        self.aligned(r, offset, 4)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.size: int = 0
        self.entries: List[AnnotationItem] = []


class AnnotationSet(DictPool):
    def __init__(self):
        super().__init__(AnnotationSetItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, annotation: Annotation = None):
        """

        :param buf:
        :param map_list_item:
        :param annotation: must not None
        :return:
        """
        if annotation is None:
            raise RuntimeError('annotation is None')
        super().parse(buf, map_list_item, annotation=annotation)
        return self


class AnnotationSetRefItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        self.dex_offset = offset.index
        annotation_set = kwargs['annotation_set']
        self.size = unpack_from('<I', buf, offset.index)[0]
        offset.index += 0x04
        if self.size:
            self.list: List[AnnotationSetItem] = []
            for annotations_off in unpack_from('<' + str(self.size) + 'I', buf, offset.index):
                self.list.append(annotation_set[annotations_off])
            offset.index += self.size * 0x04
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(pack('<I', self.size))
        if self.size:
            for item in self.list:
                r.extend(pack('<I', item.dex_offset if item else 0))

        offset.index += len(r)
        self.aligned(r, offset, 4)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.size: int = 0
        self.list: List[AnnotationSetItem] = []


class AnnotationSetRef(DictPool):
    def __init__(self):
        super().__init__(AnnotationSetRefItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, annotation_set: AnnotationSet = None):
        """

        :param buf:
        :param map_list_item:
        :param annotation_set: must not None
        :return:
        """
        if annotation_set is None:
            raise RuntimeError('annotation_set is None')
        super().parse(buf, map_list_item, annotation_set=annotation_set)
        return self


class FieldAnnotation(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        annotation_set = kwargs['annotation_set']
        self.field_idx, annotations_off = unpack_from('<2I', buf, offset.index)
        offset.index += 0x08
        self.annotations = annotation_set[annotations_off]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2I', self.field_idx, self.annotations.dex_offset if self.annotations else 0)

    def __init__(self):
        super().__init__()
        self.field_idx: int = 0
        self.annotations: Union[AnnotationSetItem, None] = None


class MethodAnnotation(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        annotation_set = kwargs['annotation_set']
        self.method_idx, annotations_off = unpack_from('<2I', buf, offset.index)
        offset.index += 0x08
        self.annotations = annotation_set[annotations_off]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2I', self.method_idx, self.annotations.dex_offset if self.annotations else 0)

    def __init__(self):
        super().__init__()
        self.method_idx: int = 0
        self.annotations: Union[AnnotationSetItem, None] = None


class ParameterAnnotation(ABCItem):
    def parse(self, buf: bytes, offset: Index, **kwargs):
        annotation_set_ref = kwargs['annotation_set_ref']
        self.method_idx, annotations_off = unpack_from('<2I', buf, offset.index)
        offset.index += 0x08
        self.annotations = annotation_set_ref[annotations_off]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<2I', self.method_idx, self.annotations.dex_offset if self.annotations else 0)

    def __init__(self):
        super().__init__()
        self.method_idx: int = 0
        self.annotations: Union[AnnotationSetRefItem, None] = None


class AnnotationsDirectoryItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        annotation_set = kwargs['annotation_set']
        annotation_set_ref = kwargs['annotation_set_ref']
        self.dex_offset = offset.index
        class_annotations_off, \
        self.annotated_fields_size, \
        self.annotated_methods_size, \
        self.annotated_parameters_size = unpack_from('<4I', buf, offset.index)
        offset.index += 0x10

        self.class_annotations = annotation_set[class_annotations_off]

        self.field_annotations: List[FieldAnnotation] = []
        for _ in range(self.annotated_fields_size):
            self.field_annotations.append(FieldAnnotation().parse(buf, offset, annotation_set=annotation_set))

        self.method_annotations: List[MethodAnnotation] = []
        for _ in range(self.annotated_methods_size):
            self.method_annotations.append(MethodAnnotation().parse(buf, offset, annotation_set=annotation_set))

        self.parameter_annotations: List[ParameterAnnotation] = []
        for _ in range(self.annotated_parameters_size):
            self.parameter_annotations.append(
                ParameterAnnotation().parse(buf, offset, annotation_set_ref=annotation_set_ref))
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        self.dex_offset = offset.index
        r = bytearray(pack('<4I',
                           self.class_annotations.dex_offset if self.class_annotations else 0,
                           self.annotated_fields_size,
                           self.annotated_methods_size,
                           self.annotated_parameters_size))
        for field in self.field_annotations:
            r.extend(field.to_bytes())

        for method in self.method_annotations:
            r.extend(method.to_bytes())

        for parameter in self.parameter_annotations:
            r.extend(parameter.to_bytes())

        offset.index += len(r)
        self.aligned(r, offset, 4)
        return bytes(r)

    def __init__(self):
        super().__init__()
        self.annotated_fields_size: int = 0
        self.annotated_methods_size: int = 0
        self.annotated_parameters_size: int = 0
        self.class_annotations: Union[AnnotationSetItem, None] = None
        self.field_annotations: List[FieldAnnotation] = []
        self.method_annotations: List[MethodAnnotation] = []
        self.parameter_annotations: List[ParameterAnnotation] = []


class AnnotationsDirectory(DictPool):
    def __init__(self):
        super().__init__(AnnotationsDirectoryItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, annotation_set: AnnotationSet = None,
              annotation_set_ref: AnnotationSetRef = None):
        """

        :param buf:
        :param map_list_item:
        :param annotation_set: must not None
        :param annotation_set_ref: must not None
        :return:
        """

        if annotation_set is None or annotation_set_ref is None:
            raise RuntimeError('annotation_set is None or annotation_set_ref is None')
        super().parse(buf, map_list_item, annotation_set=annotation_set, annotation_set_ref=annotation_set_ref)
        return self


class ClassDefItem(ABCItem):
    def parse(self, buf: bytes, offset: Index, kwargs: Dict):
        type_list = kwargs['type_list']
        annotations_directory = kwargs['annotations_directory']
        class_data = kwargs['class_data']
        encoded_array = kwargs['encoded_array']

        self.class_idx, \
        self.access_flags, \
        self.superclass_idx, \
        interfaces_off, \
        self.source_file_idx, \
        annotations_off, \
        class_data_off, \
        static_values_off = unpack_from('<8I', buf, offset.index)
        offset.index += 0x20

        self.interfaces = type_list[interfaces_off]
        self.annotations = annotations_directory[annotations_off]
        self.class_data = class_data[class_data_off]
        self.static_values = encoded_array[static_values_off]
        return self

    def to_bytes(self, offset: Index = None) -> bytes:
        return pack('<8I',
                    self.class_idx,
                    self.access_flags,
                    self.superclass_idx,
                    self.interfaces.dex_offset if self.interfaces else 0,
                    self.source_file_idx,
                    self.annotations.dex_offset if self.annotations else 0,
                    self.class_data.dex_offset if self.class_data else 0,
                    self.static_values.dex_offset if self.static_values else 0)

    def __init__(self):
        super().__init__()
        self.class_idx: int = 0
        self.access_flags: int = 0
        self.superclass_idx: int = 0
        self.source_file_idx: int = 0

        self.interfaces: Union[TypeListItem, None] = None
        self.annotations: Union[AnnotationsDirectoryItem, None] = None
        self.class_data: Union[ClassDataItem, None] = None
        self.static_values: Union[EncodedArrayItem, None] = None


class ClassDef(ListPool):
    def __init__(self):
        super().__init__(ClassDefItem)

    def parse(self, buf: bytes, map_list_item: MapListItem, type_list: TypeList = None,
              annotations_directory: AnnotationsDirectory = None, class_data: ClassData = None,
              encoded_array: EncodedArray = None):
        """

        :param buf:
        :param map_list_item:
        :param type_list: must not None
        :param annotations_directory: must not None
        :param class_data: must not None
        :param encoded_array: must not None
        :return:
        """
        if type_list is None or annotations_directory is None or class_data is None or encoded_array is None:
            raise RuntimeError('type_list is None or annotations_directory is None or class_data is None '
                               'or encoded_array is None')
        super().parse(buf, map_list_item, type_list=type_list, annotations_directory=annotations_directory,
                      class_data=class_data, encoded_array=encoded_array)
        return self


class DexHeader:
    def __init__(self):
        self.map_list_item: Union[MapListItem, None] = None
        self.magic: bytes = b''
        self.checksum: int = 0
        self.signature: bytes = b''
        self.file_size: int = 0
        self.header_size: int = 0
        self.endian_tag: int = 0
        self.link_size: int = 0
        self.link_off: int = 0
        self.map_off: int = 0
        self.string_ids_size: int = 0
        self.string_ids_off: int = 0
        self.type_ids_size: int = 0
        self.type_ids_off: int = 0
        self.proto_ids_size: int = 0
        self.proto_ids_off: int = 0
        self.field_ids_size: int = 0
        self.field_ids_off: int = 0
        self.method_ids_size: int = 0
        self.method_ids_off: int = 0
        self.class_defs_size: int = 0
        self.class_defs_off: int = 0
        self.data_size: int = 0
        self.data_off: int = 0

    def parse(self, buf: bytes, map_list_item: MapListItem):
        self.map_list_item = map_list_item
        offset = Index(self.map_list_item.dex_offset)
        self.magic: bytes = buf[offset.index:offset.index + 0x08]
        self.checksum: int = unpack_from('<I', buf, offset.index + 0x08)[0]
        self.signature: bytes = buf[offset.index + 0x0c: offset.index + 0x20]
        self.file_size, \
        self.header_size, \
        self.endian_tag, \
        self.link_size, \
        self.link_off, \
        self.map_off, \
        self.string_ids_size, \
        self.string_ids_off, \
        self.type_ids_size, \
        self.type_ids_off, \
        self.proto_ids_size, \
        self.proto_ids_off, \
        self.field_ids_size, \
        self.field_ids_off, \
        self.method_ids_size, \
        self.method_ids_off, \
        self.class_defs_size, \
        self.class_defs_off, \
        self.data_size, \
        self.data_off = unpack_from('<20I', buf, offset.index + 0x20)
        offset.index += 0x70
        return self

    def write(self, buf: bytearray, offset: Index):
        self.map_list_item.dex_offset = offset.index
        offset.index -= len(buf)
        buf.extend(self.magic)
        buf.extend(pack('<I', 0))
        buf.extend(self.signature)
        buf.extend(pack('<20I',
                        self.file_size,
                        self.header_size,
                        self.endian_tag,
                        self.link_size,
                        self.link_off,
                        self.map_off,
                        self.string_ids_size,
                        self.string_ids_off,
                        self.type_ids_size,
                        self.type_ids_off,
                        self.proto_ids_size,
                        self.proto_ids_off,
                        self.field_ids_size,
                        self.field_ids_off,
                        self.method_ids_size,
                        self.method_ids_off,
                        self.class_defs_size,
                        self.class_defs_off,
                        self.data_size,
                        self.data_off))
        offset.index += len(buf)

        self.map_list_item.offset = 0
        self.map_list_item.size = 1
        return self.map_list_item

    def update_header(self, buf: bytearray, map_list: MapList):
        self.file_size = len(buf)
        self.header_size = 0x70
        self.endian_tag = EndianTag.ENDIAN_CONSTANT.value
        self.link_size = 0
        self.link_off = 0
        self.map_off = map_list[MapListItemType.TYPE_MAP_LIST].offset
        self.string_ids_size = map_list[MapListItemType.TYPE_STRING_ID_ITEM].size
        self.string_ids_off = map_list[MapListItemType.TYPE_STRING_ID_ITEM].offset
        self.type_ids_size = map_list[MapListItemType.TYPE_TYPE_ID_ITEM].size
        self.type_ids_off = map_list[MapListItemType.TYPE_TYPE_ID_ITEM].offset
        self.proto_ids_size = map_list[MapListItemType.TYPE_PROTO_ID_ITEM].size
        self.proto_ids_off = map_list[MapListItemType.TYPE_PROTO_ID_ITEM].offset
        self.field_ids_size = map_list[MapListItemType.TYPE_FIELD_ID_ITEM].size
        self.field_ids_off = map_list[MapListItemType.TYPE_FIELD_ID_ITEM].offset
        self.method_ids_size = map_list[MapListItemType.TYPE_METHOD_ID_ITEM].size
        self.method_ids_off = map_list[MapListItemType.TYPE_METHOD_ID_ITEM].offset
        self.class_defs_size = map_list[MapListItemType.TYPE_CLASS_DEF_ITEM].size
        self.class_defs_off = map_list[MapListItemType.TYPE_CLASS_DEF_ITEM].offset
        self.data_off = \
            0x70 + \
            0x04 * self.string_ids_size + \
            0x04 * self.type_ids_size + \
            0x0c * self.proto_ids_size + \
            0x08 * self.field_ids_size + \
            0x08 * self.method_ids_size + \
            0x20 * self.class_defs_size
        self.data_size = self.file_size - self.data_off
        buf[0x20:0x70] = pack('<20I',
                              self.file_size,
                              self.header_size,
                              self.endian_tag,
                              self.link_size,
                              self.link_off,
                              self.map_off,
                              self.string_ids_size,
                              self.string_ids_off,
                              self.type_ids_size,
                              self.type_ids_off,
                              self.proto_ids_size,
                              self.proto_ids_off,
                              self.field_ids_size,
                              self.field_ids_off,
                              self.method_ids_size,
                              self.method_ids_off,
                              self.class_defs_size,
                              self.class_defs_off,
                              self.data_size,
                              self.data_off)
        buf[0x0c:0x20] = sha1(buf[0x20:]).digest()
        buf[0x08:0x0c] = pack('<I', adler32(buf[0x0c:]))


class DexFile:
    def __init__(self):
        self.dex_header: Union[DexHeader, None] = None
        self.map_list: Union[MapList, None] = None
        self.string_id: Union[StringId, None] = None
        self.type_id: Union[TypeId, None] = None
        self.type_list: Union[TypeList, None] = None
        self.proto_id: Union[ProtoId, None] = None
        self.field_id: Union[FieldId, None] = None
        self.method_id: Union[MethodId, None] = None
        self.annotation: Union[Annotation, None] = None
        self.annotation_set: Union[AnnotationSet, None] = None
        self.annotation_set_ref: Union[AnnotationSetRef, None] = None
        self.annotations_directory: Union[AnnotationsDirectory, None] = None
        self.debug_info: Union[DebugInfo, None] = None
        self.code: Union[Code, None] = None
        self.class_data: Union[ClassData, None] = None
        self.encoded_array: Union[EncodedArray, None] = None
        self.class_def: Union[ClassDef, None] = None

    def parse(self, buf: bytes):
        map_off = unpack_from('<I', buf, 0x34)[0]
        self.map_list = MapList().parse(buf, map_off)
        self.dex_header = DexHeader().parse(buf, self.map_list[MapListItemType.TYPE_HEADER_ITEM])
        self.string_id = StringId().parse(buf, self.map_list[MapListItemType.TYPE_STRING_ID_ITEM],
                                          self.map_list[MapListItemType.TYPE_STRING_DATA_ITEM])
        self.type_id = TypeId().parse(buf, self.map_list[MapListItemType.TYPE_TYPE_ID_ITEM])
        self.type_list = TypeList().parse(buf, self.map_list[MapListItemType.TYPE_TYPE_LIST])
        self.proto_id = ProtoId().parse(buf, self.map_list[MapListItemType.TYPE_PROTO_ID_ITEM], self.type_list)
        self.field_id = FieldId().parse(buf, self.map_list[MapListItemType.TYPE_FIELD_ID_ITEM])
        self.method_id = MethodId().parse(buf, self.map_list[MapListItemType.TYPE_METHOD_ID_ITEM])
        self.annotation = Annotation().parse(buf, self.map_list[MapListItemType.TYPE_ANNOTATION_ITEM])
        self.annotation_set = AnnotationSet().parse(buf,
                                                    self.map_list[MapListItemType.TYPE_ANNOTATION_SET_ITEM],
                                                    self.annotation)
        self.annotation_set_ref = AnnotationSetRef().parse(buf,
                                                           self.map_list[MapListItemType.TYPE_ANNOTATION_SET_REF_LIST],
                                                           self.annotation_set)
        self.annotations_directory = \
            AnnotationsDirectory().parse(buf,
                                         self.map_list[MapListItemType.TYPE_ANNOTATIONS_DIRECTORY_ITEM],
                                         self.annotation_set, self.annotation_set_ref)
        self.debug_info = DebugInfo().parse(buf, self.map_list[MapListItemType.TYPE_DEBUG_INFO_ITEM])
        self.code = Code().parse(buf, self.map_list[MapListItemType.TYPE_CODE_ITEM], self.debug_info)
        self.class_data = ClassData().parse(buf, self.map_list[MapListItemType.TYPE_CLASS_DATA_ITEM], self.code)
        self.encoded_array = EncodedArray().parse(buf, self.map_list[MapListItemType.TYPE_ENCODED_ARRAY_ITEM])
        self.class_def = ClassDef().parse(buf,
                                          self.map_list[MapListItemType.TYPE_CLASS_DEF_ITEM],
                                          self.type_list,
                                          self.annotations_directory,
                                          self.class_data,
                                          self.encoded_array)
        return self

    def to_bytes(self):
        index_buf = bytearray()
        index_offset = Index(0)
        data_buf = bytearray()
        data_offset = Index(self.compute_data_off())

        self.map_list[MapListItemType.TYPE_HEADER_ITEM] = self.dex_header.write(index_buf, index_offset)
        self.map_list[MapListItemType.TYPE_STRING_ID_ITEM], \
        self.map_list[MapListItemType.TYPE_STRING_DATA_ITEM] \
            = self.string_id.write(index_buf, index_offset, data_buf, data_offset)
        self.map_list[MapListItemType.TYPE_TYPE_ID_ITEM] = self.type_id.write(index_buf, index_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_TYPE_LIST] = self.type_list.write(data_buf, data_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_PROTO_ID_ITEM] = self.proto_id.write(index_buf, index_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_FIELD_ID_ITEM] = self.field_id.write(index_buf, index_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_METHOD_ID_ITEM] = self.method_id.write(index_buf, index_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_ANNOTATION_ITEM] = self.annotation.write(data_buf, data_offset)
        self.map_list[MapListItemType.TYPE_ANNOTATION_SET_ITEM] \
            = self.annotation_set.write(data_buf, data_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_ANNOTATION_SET_REF_LIST] \
            = self.annotation_set_ref.write(data_buf, data_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_ANNOTATIONS_DIRECTORY_ITEM] \
            = self.annotations_directory.write(data_buf, data_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_DEBUG_INFO_ITEM] = self.debug_info.write(data_buf, data_offset)
        self.map_list[MapListItemType.TYPE_CODE_ITEM] = self.code.write(data_buf, data_offset, aligned=4)
        self.map_list[MapListItemType.TYPE_CLASS_DATA_ITEM] = self.class_data.write(data_buf, data_offset)
        self.map_list[MapListItemType.TYPE_ENCODED_ARRAY_ITEM] = self.encoded_array.write(data_buf, data_offset)
        self.map_list[MapListItemType.TYPE_CLASS_DEF_ITEM] = self.class_def.write(index_buf, index_offset, aligned=4)

        self.map_list.write(data_buf, data_offset)
        dex_buf = index_buf + data_buf
        self.dex_header.update_header(dex_buf, self.map_list)
        return bytes(dex_buf)

    def compute_data_off(self):
        return 0x70 + \
               0x04 * self.map_list[MapListItemType.TYPE_STRING_ID_ITEM].size + \
               0x04 * self.map_list[MapListItemType.TYPE_TYPE_ID_ITEM].size + \
               0x0c * self.map_list[MapListItemType.TYPE_PROTO_ID_ITEM].size + \
               0x08 * self.map_list[MapListItemType.TYPE_FIELD_ID_ITEM].size + \
               0x08 * self.map_list[MapListItemType.TYPE_METHOD_ID_ITEM].size + \
               0x20 * self.map_list[MapListItemType.TYPE_CLASS_DEF_ITEM].size

    def get_string_by_type_idx(self, idx: int) -> str:
        return self.get_string_by_string_idx(self.type_id[idx].descriptor_idx)

    def get_method_signature(self, idx: int) -> str:
        proto: ProtoIdItem = self.proto_id[idx]
        signature = '('
        if proto.parameters:
            for type_idx in proto.parameters.list:
                signature += self.get_string_by_type_idx(type_idx)
        signature += ')' + self.get_string_by_type_idx(proto.return_type_idx)
        return signature

    def get_string_by_string_idx(self, idx: int) -> str:
        string_data: bytes = self.string_id[idx].string_data
        offset = Index(0)
        Leb128.pass_leb128(string_data, offset)
        return str(string_data[offset.index:-1], encoding='ascii')

    def get_method_name_by_method_id(self, cur_method_idx: int):
        return self.get_string_by_string_idx(self.method_id[cur_method_idx].name_idx)


class JNIFuncBuilder:
    log = logging.getLogger(__name__)

    def __init__(self, dex_path: str):
        with open(dex_path, 'rb') as reader:
            dex_buf = reader.read()
        self.dex = DexFile().parse(dex_buf)
        self.code_item: List[List[Union[str, bytes]]] = []
        self.dex_buf: bytes = b''
        self.code_items_files_buf: bytes = b''
        self.jni_func_buf: str = ''
        self.jni_func_header_buf: str = ''
        self.methods: List[Tuple[str, EncodedMethod]] = []

    def filter_methods(self, *, exclude_pkg_names: List[str] = None, include_pkg_names: List[str] = None,
                       exclude_class_names: Set[str] = None):
        """
        filter the methods
        The relationship between 'exclude_pkg_names' and 'include_pkg_names' is nor.
        :param exclude_pkg_names: pkg name's format: 'java.lang'
        :param include_pkg_names: pkg name's format: 'java.lang'
        :param exclude_class_names: class name is simple name, that is not 'java.lang.String' but 'String'
        :return: None
        :raise RuntimeError, when '(exclude_pkg_names is None and include_pkg_names is None)
        or (exclude_pkg_names and include_pkg_names)'
        """
        if (exclude_pkg_names is None and include_pkg_names is None) or (exclude_pkg_names and include_pkg_names):
            raise RuntimeError('(exclude_pkg_names is None and include_pkg_names is None) '
                               'or (exclude_pkg_names and include_pkg_names)')

        for class_def in self.dex.class_def:
            class_name = self.dex.get_string_by_type_idx(class_def.class_idx)
            pgk_name = JNIFuncBuilder.get_pkg_name(class_name)
            if exclude_class_names and class_name in exclude_class_names:
                continue

            filter_flag = True
            if exclude_pkg_names:
                for ex_pkg_name in exclude_pkg_names:
                    if pgk_name.startswith(ex_pkg_name):
                        filter_flag = True
                        break
                else:
                    filter_flag = False
            else:
                for in_pkg_name in include_pkg_names:
                    if pgk_name.startswith(in_pkg_name):
                        filter_flag = False
                        break
                else:
                    filter_flag = True

            if not filter_flag:
                class_data = class_def.class_data
                if class_data is None:
                    continue
                cur_method_idx = 0
                for method in class_data.direct_methods:
                    cur_method_idx += method.method_idx_diff
                    method_name = self.dex.get_method_name_by_method_id(cur_method_idx)
                    if method.code and method_name != r'<clinit>' and method_name != r'<init>':
                        copy_method = class_name, self.copy_and_native(method, cur_method_idx)
                        self.methods.append(copy_method)
                cur_method_idx = 0
                for method in class_data.virtual_methods:
                    cur_method_idx += method.method_idx_diff
                    if method.code:
                        copy_method = class_name, self.copy_and_native(method, cur_method_idx)
                        self.methods.append(copy_method)

    def write_to(self, code_item_out_path: str,
                 dex_out_path: str,
                 jni_func_out_path: str,
                 jni_func_header_out_path: str):
        self.dex_buf = bytearray(self.dex.to_bytes())
        with open(dex_out_path, 'wb') as writer:
            writer.write(self.dex_buf)
            writer.flush()

        # self.set_code_item_zero(dex_out_path)
        self.format_code_item()
        with open(code_item_out_path, 'wb') as writer:
            writer.write(self.code_items_files_buf)
            writer.flush()

        end_index = jni_func_header_out_path.rfind(r'/')
        if end_index == -1:
            end_index = jni_func_header_out_path.rfind('\\')
        self.generate_jni_func(jni_func_header_out_path[end_index + 1:])
        with open(jni_func_out_path, 'w') as writer:
            writer.write(self.jni_func_buf)
            writer.flush()

        with open(jni_func_header_out_path, 'w') as writer:
            writer.write(self.jni_func_header_buf)
            writer.flush()

    def format_code_item(self):
        """

        :return: the bytes of code item index struct
                struct_size:                                                    :uint_32
                struct:
                    key_size                                                    :uint_32
                    (sign + method_name + class_name).reverse                   :str
                    value_size                                                  :uint_32
                    method.code.to_bytes()                                      :str
        """

        code_items_files_buf = bytearray()
        code_items_files_buf.extend(pack('<I', len(self.methods)))
        for class_name, method in self.methods:
            method_id: MethodIdItem = self.dex.method_id[method.method_idx_diff]
            method_name = self.dex.get_string_by_string_idx(method_id.name_idx)
            sign = self.dex.get_method_signature(method_id.proto_idx)

            key = bytearray(sign + method_name + class_name, encoding='ascii')
            self.log.debug('code item key0: %s', key)
            key.reverse()
            self.log.debug('code item key : %s', key)
            key.append(0)
            key_size = len(key)
            self.log.debug('      key size: %d', key_size)
            code_items_files_buf.extend(pack('<I', key_size))
            code_items_files_buf.extend(key)

            # reserved 16B for two pointers (64 bit operating system) after insns_size,
            # in order to read bytes by c++ pointer conveniently.
            value = method.code.to_bytes(Index(0))
            value = value[:0x10] + bytes(0x10) + value[0x10:]
            value_size = len(value)
            code_items_files_buf.extend(pack('<I', value_size))
            code_items_files_buf.extend(value)

        self.code_items_files_buf = bytes(code_items_files_buf)

    def generate_jni_func(self, jni_func_header_name: str):
        self.jni_func_buf = r"""
                 
            //
            // Created by ChenDalunlun on {today_date}.
            //
            
            #include "{jni_func_header_name}"
            #include "VMInterpreter.h"
            #include "Util.h"
            #include "common/Dalvik.h"
                    
        """.format(today_date=str(date.today()), jni_func_header_name=jni_func_header_name)

        self.jni_func_header_buf = r"""
            
            //
            // Created by ChenDalunlun on {today_date}.
            //
            
            #ifndef SHELLAPPLICATION_JNIFUNC_H
            #define SHELLAPPLICATION_JNIFUNC_H
            
            #include <jni.h>
                    
            using namespace std;
        
        """.format(today_date=str(date.today()))

        for class_name, method in self.methods:
            method_id: MethodIdItem = self.dex.method_id[method.method_idx_diff]
            method_name = self.dex.get_string_by_string_idx(method_id.name_idx)
            sign = self.dex.get_method_signature(method_id.proto_idx)

            proto_id: ProtoIdItem = self.dex.proto_id[method_id.proto_idx]
            return_type = JNIFuncBuilder.format_jni_type(self.dex.get_string_by_type_idx(proto_id.return_type_idx))
            class_name_jni = JNIFuncBuilder.format_jni_name(class_name[1:-1])
            method_name_jni = JNIFuncBuilder.format_jni_name(method_name)
            param_sign_jni = ''
            if proto_id.parameters:
                for param_type_id in proto_id.parameters.list:
                    param_sign_jni += self.dex.get_string_by_type_idx(param_type_id)
                param_sign_jni = JNIFuncBuilder.format_jni_name(param_sign_jni)

            object_or_class = 'jclass clazz_type' if method.access_flags & AccessFlag.ACC_STATIC else 'jobject instance'

            param_define_jni = ''
            if proto_id.parameters:
                for param_no, param_type_id in enumerate(proto_id.parameters.list):
                    param_define_jni += ', ' + JNIFuncBuilder.format_jni_type(
                        self.dex.get_string_by_type_idx(param_type_id)) + ' param{no}'.format(no=param_no)
            func_declare = """
                
                extern "C"
                JNIEXPORT {return_type} JNICALL
                Java_{class_name_jni}_{method_name_jni}__{param_sign_jni}(
                        JNIEnv *env, {object_or_class}{param_define_jni})
            """.format(return_type=return_type,
                       class_name_jni=class_name_jni,
                       method_name_jni=method_name_jni,
                       param_sign_jni=param_sign_jni,
                       object_or_class=object_or_class,
                       param_define_jni=param_define_jni)

            self.jni_func_buf += func_declare
            self.jni_func_header_buf += func_declare + ';\n'

            self.jni_func_buf += """{
                
                LOG_D("jni function start: %s", __func__);
            """
            if method.access_flags & AccessFlag.ACC_STATIC:
                self.jni_func_buf += r'    jobject instance = clazz_type;'
            else:
                self.jni_func_buf += r'    jclass clazz_type = (*env).GetObjectClass(instance);'

            param = ''
            if proto_id.parameters:
                for param_no in range(len(proto_id.parameters.list)):
                    param += ', param{no}'.format(no=param_no)

            self.jni_func_buf += """
                jmethodID m_{method_name} = (*env).GetMethodID(clazz_type, "{method_name}", "{method_sign}");
                // init method
                const VmMethod *method = initVmMethod(m_{method_name});
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue{param});
                deleteVmMethod(method);
                LOG_D("jni function finish");
            """.format(method_name=method_name, method_sign=sign, param=param)

            jvalue_type = JNIFuncBuilder.format_jvalue_type(self.dex.get_string_by_type_idx(proto_id.return_type_idx))
            if jvalue_type:
                self.jni_func_buf += """
                    return retValue.{jvalue_type};
                """.format(jvalue_type=jvalue_type)

            self.jni_func_buf += '}'

        self.jni_func_header_buf += '    #endif //SHELLAPP_JNIFUNC_H'

    @staticmethod
    def format_jni_name(name: str) -> str:
        return name.replace('_', '_1') \
            .replace(';', '_2') \
            .replace('[', '_3') \
            .replace('/', '_').replace('.', '_') \
            .replace('$', '_00024')

    @staticmethod
    def format_jvalue_type(java_type: str) -> Union[str, None]:
        if java_type[0] == 'I':
            return 'i'
        elif java_type[0] == 'B':
            return 'b'
        elif java_type[0] == 'Z':
            return 'z'
        elif java_type[0] == 'S':
            return 's'
        elif java_type[0] == 'C':
            return 'c'
        elif java_type[0] == 'D':
            return 'd'
        elif java_type[0] == 'F':
            return 'f'
        elif java_type[0] == 'J':
            return 'j'
        elif java_type[0] == 'L' or java_type[0] == '[':
            return 'l'
        else:
            return None

    @staticmethod
    def format_jni_type(java_type: str) -> str:
        if java_type[0] == 'I':
            return 'jint'
        elif java_type[0] == 'B':
            return 'jbyte'
        elif java_type[0] == 'Z':
            return 'jboolean'
        elif java_type[0] == 'S':
            return 'jshort'
        elif java_type[0] == 'C':
            return 'jchar'
        elif java_type[0] == 'D':
            return 'jdouble'
        elif java_type[0] == 'F':
            return 'jfloat'
        elif java_type[0] == 'J':
            return 'jlong'
        elif java_type[0] == 'L' or java_type[0] == '[':
            return 'jobject'
        elif java_type[0] == 'V':
            return 'void'

    # def set_code_item_zero(self, dex_out_path: str):
    #     with open(dex_out_path, 'rb') as reader:
    #         dex_buf = bytearray(reader.read())
    #         for _, method in self.methods:
    #             offset = Index(method.code.dex_offset)
    #             CodeItem.pass_item(dex_buf, offset)
    #             dex_buf[method.code.dex_offset:offset.index] = bytes(offset.index - method.code.dex_offset)
    #
    #         # update signature and checksum
    #         dex_buf[0x0c:0x20] = sha1(dex_buf[0x20:]).digest()
    #         dex_buf[0x08:0x0c] = pack('<I', adler32(dex_buf[0x0c:]))
    #
    #     os.remove(dex_out_path)
    #     with open(dex_out_path, 'wb') as writer:
    #         # write new dex_buf, which special code item was set to zero
    #         writer.write(dex_buf)
    #         writer.flush()

    @staticmethod
    def get_pkg_name(class_name: str) -> str:
        return class_name[1:class_name.rfind('/')].replace('/', '.')

    def copy_and_native(self, method: EncodedMethod, cur_method_idx: int) -> EncodedMethod:
        new_method = EncodedMethod()
        new_method.method_idx_diff = cur_method_idx
        new_method.access_flags = method.access_flags
        new_method.code = method.code

        method.access_flags = method.access_flags | AccessFlag.ACC_NATIVE
        self.remove_code_and_debug_info(method)
        return new_method

    def remove_code_and_debug_info(self, method: EncodedMethod):
        # remove debug info item from pool
        if method.code.debug_info:
            self.dex.debug_info.pool.pop(method.code.debug_info.dex_offset)
            method.code.debug_info = None

        # remove code item from pool
        self.dex.code.pool.pop(method.code.dex_offset)
        method.code = None
