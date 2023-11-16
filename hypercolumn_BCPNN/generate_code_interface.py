import re

class Parameters:
    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)

def parse_cpp_header(header_text):
    # Define a regular expression to match any variable name and its value
    regex = r"const\s+(\w+::\w+|\w+)\s+(\w+)\s*=\s*(.+);"

    # Use re.findall to find all matches in the header_text
    matches = re.findall(regex, header_text)

    # Create a dictionary to store the variable names and values
    param_dict = {}
    for match in matches:
        var_type, var_name, var_value = match
        if var_type == "std::string":
            # Remove quotes for string values
            var_value = var_value.strip('\"')
        elif var_type == "float":
            var_value = float(var_value)
        elif var_type == "int":
            var_value = int(var_value)

        # Add the variable name and value to the dictionary
        param_dict[var_name] = var_value

    # Create and return a Parameters object
    return Parameters(**param_dict)

def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()
    
cpp_param = read_file("model_param.h")

param = parse_cpp_header(cpp_param)
# if param:
#     for key, value in param.__dict__.items():
#         print(f"{key}: {value}")
# else:
#     print("Header parsing failed.")

cpp_interface = ""
cpp_interface_head = "#include \"hypercolumn_CODE/definitions.h\"\n"

# generate code strings
allocatefiringProb = "typedef void (*allocatefiringProb)(unsigned int); \n"
allocatefiringProb += "allocatefiringProb allocatefiringProbs[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            allocatefiringProb += "    allocatefiringProbH" + str(i) + "_" + str(j) + "_M" + str(m) + "_input,\n"
allocatefiringProb += "};\n"

pushfiringProbToDevice = "typedef void (*pushfiringProbToDevice)(unsigned int); \n"
pushfiringProbToDevice += "pushfiringProbToDevice pushfiringProbsToDevice[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            pushfiringProbToDevice += "    pushfiringProbH" + str(i) + "_" + str(j) + "_M" + str(m) + "_inputToDevice,\n"
pushfiringProbToDevice += "};\n"

firingProb = "scalar** firingProbs[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            firingProb += "    &firingProbH" + str(i) + "_" + str(j) + "_M" + str(m) + "_input,\n"
firingProb += "};\n"

recordSpk = "uint32_t** recordSpkArray[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            recordSpk += "    &recordSpkH" + str(i) + "_" + str(j) + "_M" + str(m) + ",\n"
recordSpk += "};\n"

wGain = "scalar** wGains[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            for ip in range(param.hyper_height):
                for jp in range(param.hyper_width):
                    for mp in range(param.N_minicolumns):
                        wGain +=   "    &wGainH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmda,\n"     # wGainH0_0_to_H0_0_M0to_0_lateral_nmda
                        wGain +=   "    &wGainH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampa,\n"
wGain += "};\n"

kappa = "scalar** kappas[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            for ip in range(param.hyper_height):
                for jp in range(param.hyper_width):
                    for mp in range(param.N_minicolumns):
                        kappa +=   "    &kappaH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmda,\n"     # wGainH0_0_to_H0_0_M0to_0_lateral_nmda
                        kappa +=   "    &kappaH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampa,\n"
kappa += "};\n"

pushwGain = "typedef void (*pushwGain)(bool); \n"
pushwGain += "pushwGain pushwGains[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            for ip in range(param.hyper_height):
                for jp in range(param.hyper_width):
                    for mp in range(param.N_minicolumns):
                        pushwGain +=   "    pushwGainH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmdaToDevice,\n"     # pushwGainH0_0_to_H0_0_M0to_0_lateral_ampaToDevice
                        pushwGain +=   "    pushwGainH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampaToDevice,\n"
pushwGain += "};\n"

pushkappa = "typedef void (*pushkappa)(bool); \n"
pushkappa += "pushkappa pushkappas[] = {\n"
for i in range(param.hyper_height):
    for j in range(param.hyper_width):
        for m in range(param.N_minicolumns):
            for ip in range(param.hyper_height):
                for jp in range(param.hyper_width):
                    for mp in range(param.N_minicolumns):
                        pushkappa +=   "    pushkappaH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmdaToDevice,\n"     # pushkappaH0_0_to_H0_0_M0to_0_lateral_ampaToDevice
                        pushkappa +=   "    pushkappaH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampaToDevice,\n"
pushkappa += "};\n"



cpp_interface += cpp_interface_head
cpp_interface += allocatefiringProb
cpp_interface += pushfiringProbToDevice
cpp_interface += firingProb
cpp_interface += recordSpk
cpp_interface += wGain
cpp_interface += kappa
cpp_interface += pushwGain
cpp_interface += pushkappa




with open("code_interface.h", 'w') as file:
    # Write content to the file
    file.write(cpp_interface)
