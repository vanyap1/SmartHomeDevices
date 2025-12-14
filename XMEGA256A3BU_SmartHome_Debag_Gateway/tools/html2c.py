def html_to_c_string(filename):
    with open(filename, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    c_string = 'const char psu_page[] PROGMEM = "HTTP/1.1 200 OK\\r\\nContent-Type: text/html\\r\\n\\r\\n'
    for line in lines:
        stripped_line = line.strip().replace('"', '\\"')
        c_string += stripped_line + '\\n'
    c_string += '";'

    return c_string

html_file = 'index.html'
c_string = html_to_c_string(html_file)
with open('html_page.c', 'w', encoding='utf-8') as file:
    file.write(c_string)

# Generate report
import os
html_size = os.path.getsize(html_file)
c_size = os.path.getsize('html_page.c')
c_string_length = len(c_string)

print(f'HTML to C conversion completed successfully!')
print(f'─' * 50)
print(f'Source file: {html_file}')
print(f'Output file: html_page.c')
print(f'─' * 50)
print(f'HTML file size: {html_size:,} bytes')
print(f'C file size: {c_size:,} bytes')
print(f'C string length: {c_string_length:,} characters')
print(f'Compression ratio: {c_size/html_size:.2f}x')
print(f'─' * 50)
print(f'Ready for embedding in microcontroller firmware')
c_string = html_to_c_string(html_file)
with open('html_page.c', 'w', encoding='utf-8') as file:  # <== і тут
    file.write(c_string)

print(f'HTML content from {html_file} has been converted and saved to html_page.c')
