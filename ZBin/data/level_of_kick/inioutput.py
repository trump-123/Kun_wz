def add_iniMsg(ini,id,A_c,A_f):
    ini.write("[Robot" + str(id) + "]\n")

    ini.write("CHIP_A = " + str(A_c[0]) + "\n")

    ini.write("CHIP_B = " + str(A_c[1]) + "\n")

    ini.write("CHIP_C = " + str(A_c[2]) + "\n")

    ini.write("CHIP_MAX = 127 \nCHIP_MIN = 0 \n")

    ini.write("FLAT_A = " + str(A_f[0]) + "\n")

    ini.write("FLAT_B = " + str(A_f[1]) + "\n")

    ini.write("FLAT_C = " + str(A_f[2]) + "\n")

    ini.write("FLAT_MAX = 127 \nFLAT_MIN = 20 \n")

    ini.write("\n")
    pass