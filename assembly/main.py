if __name__ == '__main__':
    with open('small', 'rb') as file:
        bytes = file.read()

        content = ''.join(format(byte, '08b') for byte in bytes)

        #print(bytes.decode('utf-8', errors='ignore'))

        for i in range(0, len(content), 32):
            print(f'{content[i:i + 31]}')

        print(len(content) / 32)