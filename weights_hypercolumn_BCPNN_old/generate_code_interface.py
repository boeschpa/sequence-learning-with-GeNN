import re
# generates code_interface.cpp to interface with the pointers to neuron and synapse populations 
# that are generated by GeNN
# code_interface.cpp provides arrays of the pointers that are declared in XXXXX_code/definitions.h

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

# gH0_0_to_H0_0_M0to_0_lateral_ampa
g_nmda = "scalar** g_nmda[] = {\n"
for mp in range(param.N_minicolumns):
    for m in range(param.N_minicolumns):
        for i in range(param.hyper_height):
            for j in range(param.hyper_width):
                for ip in range(param.hyper_height):
                    for jp in range(param.hyper_width):
                        g_nmda +=   "    &gH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmda,\n"
g_nmda += "};\n"

g_ampa = "scalar** g_ampa[] = {\n"
for mp in range(param.N_minicolumns):
    for m in range(param.N_minicolumns):
        for i in range(param.hyper_height):
            for j in range(param.hyper_width):
                for ip in range(param.hyper_height):
                    for jp in range(param.hyper_width):
                        g_ampa +=   "    &gH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampa,\n"
g_ampa += "};\n"

maxRowLength = "const unsigned int* maxRowLengths[] = {\n"
for mp in range(param.N_minicolumns):
    for m in range(param.N_minicolumns):        
        for i in range(param.hyper_height):
            for j in range(param.hyper_width):
                for ip in range(param.hyper_height):
                    for jp in range(param.hyper_width):
                        maxRowLength +=   "    &maxRowLengthH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_nmda,\n"
                        maxRowLength +=   "    &maxRowLengthH" + str(i) + "_" + str(j) + "_to_H" + str(ip) + "_" + str(jp) + \
                                        "_M" + str(m) + "to_" + str(mp) + "_lateral_ampa,\n"
maxRowLength += "};\n"

biasGain = "scalar** biasGains[] = {\n"
for m in range(param.N_minicolumns):  
    for i in range(param.hyper_height):
        for j in range(param.hyper_width):
            biasGain +=   "    &biasGainH" + str(i) + "_" + str(j) + "_M" + str(m) + ",\n"
biasGain += "};\n"

kappaBias = "scalar** kappasBias[] = {\n"
for m in range(param.N_minicolumns):  
    for i in range(param.hyper_height):
        for j in range(param.hyper_width):
            kappaBias +=   "    &kappaH" + str(i) + "_" + str(j) + "_M" + str(m) + ",\n"
kappaBias += "};\n"

pushbiasGain = "typedef void (*pushbiasGain)(bool); \n"
pushbiasGain += "pushbiasGain pushbiasGains[] = {\n"
for m in range(param.N_minicolumns):  
    for i in range(param.hyper_height):
        for j in range(param.hyper_width):
            pushbiasGain +=   "    pushbiasGainH" + str(i) + "_" + str(j) + "_M" + str(m) + "ToDevice,\n"  # add bias gain push
pushbiasGain += "};\n"

pushkappaBias = "typedef void (*pushkappaBias)(bool); \n"
pushkappaBias += "pushkappaBias pushkappasBias[] = {\n"
for m in range(param.N_minicolumns):  
    for i in range(param.hyper_height):
        for j in range(param.hyper_width):
            pushkappaBias +=   "    pushkappaH" + str(i) + "_" + str(j) + "_M" + str(m) + "ToDevice,\n"  # add bias kappa push
pushkappaBias += "};\n"



cpp_interface += cpp_interface_head
cpp_interface += allocatefiringProb
cpp_interface += pushfiringProbToDevice
cpp_interface += firingProb
cpp_interface += recordSpk
cpp_interface += wGain
cpp_interface += kappa
cpp_interface += pushwGain
cpp_interface += pushkappa
cpp_interface += g_nmda
cpp_interface += g_ampa
cpp_interface += maxRowLength
cpp_interface += biasGain
cpp_interface += kappaBias
cpp_interface += pushbiasGain
cpp_interface += pushkappaBias




with open("code_interface.h", 'w') as file:
    # Write content to the file
    file.write(cpp_interface)
