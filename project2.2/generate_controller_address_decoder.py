table = ["add", "mul", "sub", "sll", "mulh", "mulhu", "xor", "div", "divu", "srl", "or", "remu", "and", "lb", "lh", "lw", "lbu", "lhu", 
         "addi", "slli", "slti", "xori", "srli", "ori", "andi", "sw", "beq", "blt", "bltu", "bne", "bgeu", "auipc", "lui", "jal", "jalr"]
type_table = ["R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "I", "I", "I", "I", "I", "I", "I", "I", "I", "I", "I", "I", 
              "S", "B", "B", "B", "B", "B", "U", "U", "J", "I"]
inst_table = [[0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x03, 0x03, 0x03, 0x03, 0x03, 0x13, 0x13, 
                0x13, 0x13, 0x13, 0x13, 0x13, 0x23, 0x63, 0x63, 0x63, 0x63, 0x63, 0x17, 0x37, 0x6f, 0x67], 
              [0x0, 0x0, 0x0, 0x1, 0x1, 0x3, 0x4, 0x4, 0x5, 0x5, 0x6, 0x7, 0x7, 0x0, 0x1, 0x2, 0x4, 0x5, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6,
                0x7, 0x2, 0x0, 0x4, 0x6, 0x1, 0x7, -1, -1, -1, 0x0],
              [0x00, 0x01, 0x20, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, -1, -1, -1, -1, -1, -1, 0x00, 
                -1, -1, 0x00, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1]]
inst_range_table = [(2, 7), (12, 15), (25, 32)]
inst_begin_table = [0, 12, 25]
opcode_table = [0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x03, 0x03, 0x03, 0x03, 0x03, 0x13, 0x13, 
                0x13, 0x13, 0x13, 0x13, 0x13, 0x23, 0x63, 0x63, 0x63, 0x63, 0x63, 0x17, 0x37, 0x6f, 0x67]
funct3_table = [0x0, 0x0, 0x0, 0x1, 0x1, 0x3, 0x4, 0x4, 0x5, 0x5, 0x6, 0x7, 0x7, 0x0, 0x1, 0x2, 0x4, 0x5, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6,
                0x7, 0x2, 0x0, 0x4, 0x6, 0x1, 0x7, -1, -1, -1, 0x0]
funct7_table = [0x00, 0x01, 0x20, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, -1, -1, -1, -1, -1, -1, 0x00, 
                -1, -1, 0x00, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1]
# order = {2 : 0, 3 : 1, 4 : 2, 5 : 3, 6 : 4, 12 : 5, 13 : 6, 14 : 7, 30 : 8}
order = {2 : 0, 3 : 1, 4 : 2, 5 : 3, 6 : 4, 12 : 5, 13 : 6, 14 : 7, 25 : 8, 26 : 9, 27 : 10, 28 : 11, 29 : 12, 30 : 13, 31 : 14}

Delta = 790
delta = 50
for i in range(0, 35):
    
    and_x = 1620
    and_y = 530 + Delta * i + 30
    print('<comp lib="1" loc="({0},{1})" name="AND Gate">'.format(and_x, and_y))
    print('<a name="inputs" val="15"/>')
    print('</comp>')

    flag_tunnel_x = and_x + 30
    flag_tunnel_y = and_y
    print('<comp lib="0" loc="({0},{1})" name="Tunnel">'.format(flag_tunnel_x, flag_tunnel_y))
    print('<a name="label" val="{0}_flag"/>'.format(table[i]))
    print('</comp>')

    print('<wire from="({0},{1})" to="({2},{3})"/>'.format(and_x, and_y, flag_tunnel_x, flag_tunnel_y))

    for k in range(0, 3):
        for j in range(inst_range_table[k][0], inst_range_table[k][1]):
            bit = (inst_table[k][i] >> (j - inst_begin_table[k])) & 1

            tunnel_x = 1090
            tunnel_y = 510 + i * Delta + order[j] * delta
            print('<comp lib="0" loc="({0},{1})" name="Tunnel">'.format(tunnel_x, tunnel_y))
            print('<a name="facing" val="east"/>')
            print('<a name="label" val="bit_{0}"/>'.format(j))
            print('</comp>')
            not_x = tunnel_x + 60
            not_y = tunnel_y
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(tunnel_x, tunnel_y, not_x - 20, not_y))
            if bit == 0:
                print('<comp lib="1" loc="({0},{1})" name="NOT Gate">'.format(not_x, not_y))
                print('<a name="size" val="20"/>')
                print('</comp>')
            else:
                print('<wire from="({0},{1})" to="({2},{3})"/>'.format(not_x - 20, not_y, not_x, not_y))
            or_up_x = not_x + 70
            or_up_y = tunnel_y
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(not_x, not_y, or_up_x, or_up_y))

            const_x = not_x
            const_y = 530 + i * Delta + order[j] * delta

            unused = int(inst_table[k][i] == -1)

            print('<comp lib="0" loc="({0},{1})" name="Constant">'.format(const_x, const_y))
            print('<a name="value" val="0x{}"/>'.format(unused))
            print('</comp>')

            or_down_x = const_x + 70
            or_down_y = const_y
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(const_x, const_y, or_down_x, or_down_y))

            or_x = or_up_x + 30
            or_y = or_up_y + 10
            print('<comp lib="1" loc="({0},{1})" name="OR Gate">'.format(or_x, or_y))
            print('<a name="size" val="30"/>')
            print('</comp>')

            node_1_x = 1270 + 10 * order[j]
            node_1_y = or_y
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(or_x, or_y, node_1_x, node_1_y))

            node_2_x = node_1_x
            node_2_y = 490 + 10 * order[j] + Delta * i
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(node_2_x, node_2_y, node_1_x, node_1_y))

            node_3_x = 1570
            node_3_y = node_2_y
            print('<wire from="({0},{1})" to="({2},{3})"/>'.format(node_2_x, node_2_y, node_3_x, node_3_y))
