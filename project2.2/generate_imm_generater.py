for i in range(1,8):
    # print('<wire from="(370,{0}0)" to="(400,{0}0)"/>'.format(i))
    print('<wire from="({0},170)" to="({0},240)"/>'.format(1100+90*i))
    print('<comp lib="0" loc="({0},240)" name="Tunnel">'.format(1100+90*i))
    print('<a name="facing" val="north"/>')
    print('<a name="width" val="32"/>')
    print('<a name="label" val="t"/>')
    print('</comp>')