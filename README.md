CSV ↔ Binary ↔ XML Converter

This project is a C-based application that performs CSV to Binary conversion, Binary to XML conversion, and XML validation with XSD schema.

📂 Project Structure

2021510107.c → The main driver program. It parses command-line arguments and calls the appropriate conversion program
.

csv_to_binary.c → Reads a CSV file and converts it into Binary format. Missing fields are logged into skipped_fields.save_entry
.

binary_to_xml.c → Reads a Binary file (defined in setupParams.json), sorts the records, and exports them into an XML format
.

validate (not provided as source, but called by 2021510107.c) → Validates an XML file against an XSD schema.

⚙️ Compilation

To compile all programs on Linux:

gcc 2021510107.c -o 2021510107
gcc csv_to_binary.c -o csv_to_binary
gcc binary_to_xml.c -o binary_to_xml $(pkg-config --cflags --libs libxml-2.0 json-c)
gcc validate.c -o validate $(pkg-config --cflags --libs libxml-2.0)


Note: Requires libxml2 and json-c libraries.

🚀 Usage

General usage:

./2021510107 <input_file> <output_file> <conversion_type> -separator <1|2|3> -opsys <1|2|3> [-h]

Arguments

<input_file> → Source file

<output_file> → Target file

<conversion_type>:

1 → CSV → Binary

2 → Binary → XML (binary filename read from setupParams.json)

3 → XML validation with XSD

-separator <1|2|3> → Field separator: 1=, (comma), 2=\t (tab), 3=; (semicolon)

-opsys <1|2|3> → Line endings: 1=Windows, 2=Linux, 3=MacOS

-h → Displays help message

Examples

Convert CSV to Binary:

./2021510107 smartlog.csv logdata.dat 1 -separator 1 -opsys 2


Convert Binary to XML:

./2021510107 logdata.dat smartlogs.xml 2 -separator 1 -opsys 2


Validate XML with XSD:

./2021510107 smartlogs.xml 2021510107.xsd 3 -separator 1 -opsys 2

📑 Important Files

setupParams.json → Defines the binary filename, sort key range, and sort order for Binary → XML conversion.

skipped_fields.save_entry → Contains logs of missing fields when converting CSV → Binary.

🛠️ Workflow

Convert raw sensor logs in CSV format → Binary format.

Convert Binary file → XML format (with sorting and structure).

Validate XML file → against XSD schema for correctness.

Do you want me to also prepare this as a ready-to-use README.md file (Markdown-formatted) that you can drop straight into your project?
