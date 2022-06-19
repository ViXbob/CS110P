table = ["add", "mul", "sub", "sll", "mulh", "mulhu", "xor", "div", "divu", "srl", "or", "remu", "and", "lb", "lh", "lw", "lbu", "lhu", 
         "addi", "slli", "slti", "xori", "srli", "ori", "andi", "sw", "beq", "blt", "bltu", "bne", "bgeu", "auipc", "lui", "jal", "jalr"]
for i in range(0, 35):
    tunnel_x = 1210
    tunnel_y = 150 + i * 20
    pin_x = 400
    pin_y = tunnel_y
    # print('<comp lib="0" loc="({0},{1})" name="Tunnel">'.format(tunnel_x, tunnel_y))
    # print('<a name="facing" val="east"/>')
    # print('<a name="label" val="{}"/>'.format(table[i] + "_flag"))
    # print('</comp>')
    # <comp lib="0" loc="(1210,150)" name="Tunnel">
    #   <a name="facing" val="east"/>
    #   <a name="label" val="add_flag"/>
    # </comp>
    print('<comp lib="0" loc="({0},{1})" name="Tunnel">'.format(tunnel_x, tunnel_y))
    print('<a name="facing" val="east"/>')
    print('<a name="label" val="{}_flag"/>'.format(table[i]))
    print('</comp>')

    # print('<wire from="({0},{1})" to="({2},{3})"/>'.format(tunnel_x, tunnel_y, pin_x, pin_y))
