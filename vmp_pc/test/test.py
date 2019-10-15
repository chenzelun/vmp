import os

in_path_dir = r'C:\Users\dalunlun\Documents\czl_git\vmp\vmpApplication\app\src\main\cpp'

key = [
    'FindClass',
    'GetObjectField',
    'GetObjectClass',
    'GetObjectArrayElement'
    'GetStringUTFChars',
    'New'
]

for file in os.listdir(in_path_dir):
    if not file.endswith('.cpp'):
        continue

    print('\n\n\n\n')
    print(in_path_dir + '/' + file)
    with open(in_path_dir + '/' + file, 'r', encoding='utf8') as f:
        for line_num, line in enumerate(f.readlines()):
            for k in key:
                if k in line:
                    print('line_num: ' + str(line_num + 1) + ', ' + line)

print('ok')
