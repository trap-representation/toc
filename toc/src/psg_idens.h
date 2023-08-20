#ifdef TOC_ADD_KEYWORD
#ifndef TOC_ADDED_KEYWORD
#define TOC_ADDED_KEYWORD

// keyword
#define key_pushc 0
#define key_pushs 1
#define key_pushi 2
#define key_pushp 3
#define key_pushl 4
#define key_pushuc 5
#define key_pushus 6
#define key_pushui 7
#define key_eq 8
#define key_gt 9
#define key_ls 10
#define key_ge 11
#define key_le 12
#define key_ne 13
#define key_zr 14
#define key_nz 15
#define key_addc 16
#define key_adds 17
#define key_addi 18
#define key_addp 19
#define key_addl 20
#define key_adduc 21
#define key_addus 22
#define key_addui 23
#define key_subc 24
#define key_subs 25
#define key_subi 26
#define key_subp 27
#define key_subl 28
#define key_subuc 29
#define key_subus 30
#define key_subui 31
#define key_mulc 32
#define key_muls 33
#define key_muli 34
#define key_mull 35
#define key_muluc 36
#define key_mulus 37
#define key_mului 38
#define key_divc 39
#define key_divs 40
#define key_divi 41
#define key_divl 42
#define key_divuc 43
#define key_divus 44
#define key_divui 45
#define key_remc 46
#define key_rems 47
#define key_remi 48
#define key_reml 49
#define key_remuc 50
#define key_remus 51
#define key_remui 52
#define key_lshc 53
#define key_lshs 54
#define key_lshi 55
#define key_lshl 56
#define key_lshuc 57
#define key_lshus 58
#define key_lshui 59
#define key_rshc 60
#define key_rshs 61
#define key_rshi 62
#define key_rshl 63
#define key_rshuc 64
#define key_rshus 65
#define key_rshui 66
#define key_orc 67
#define key_ors 68
#define key_ori 69
#define key_orl 70
#define key_oruc 71
#define key_orus 72
#define key_orui 73
#define key_andc 74
#define key_ands 75
#define key_andi 76
#define key_andl 77
#define key_anduc 78
#define key_andus 79
#define key_andui 80
#define key_notc 81
#define key_nots 82
#define key_noti 83
#define key_notl 84
#define key_notuc 85
#define key_notus 86
#define key_notui 87
#define key_xorc 88
#define key_xors 89
#define key_xori 90
#define key_xorl 91
#define key_xoruc 92
#define key_xorus 93
#define key_xorui 94
#define key_swap 95
#define key_pop 96
#define key_dup 97
#define key_ret 98
#define key_over 99
#define key_panic 100
#define key_write0 101
#define key_write1 102
#define key_write2 103
#define key_write3 104
#define key_write4 105
#define key_write5 106
#define key_write6 107
#define key_write7 108
#define key_vwrite0 109
#define key_vwrite1 110
#define key_vwrite2 111
#define key_vwrite3 112
#define key_vwrite4 113
#define key_vwrite5 114
#define key_vwrite6 115
#define key_vwrite7 116
#define key_get0 117
#define key_get1 118
#define key_get2 119
#define key_get3 120
#define key_get4 121
#define key_get5 122
#define key_get6 123
#define key_get7 124
#define key_vget0 125
#define key_vget1 126
#define key_vget2 127
#define key_vget3 128
#define key_vget4 129
#define key_vget5 130
#define key_vget6 131
#define key_vget7 132
#define key_loadc 133
#define key_loads 134
#define key_loadi 135
#define key_loadp 136
#define key_loadl 137
#define key_loaduc 138
#define key_loadus 139
#define key_loadui 140
#define key_vloadc 141
#define key_vloads 142
#define key_vloadi 143
#define key_vloadp 144
#define key_vloadl 145
#define key_vloaduc 146
#define key_vloadus 147
#define key_vloadui 148
#define key_aloadc 149
#define key_aloads 150
#define key_aloadi 151
#define key_aloadp 152
#define key_aloadl 153
#define key_aloaduc 154
#define key_aloadus 155
#define key_aloadui 156
#define key_valoadc 157
#define key_valoads 158
#define key_valoadi 159
#define key_valoadp 160
#define key_valoadl 161
#define key_valoaduc 162
#define key_valoadus 163
#define key_valoadui 164
#define key_storec 165
#define key_stores 166
#define key_storei 167
#define key_storep 168
#define key_storel 169
#define key_storeuc 170
#define key_storeus 171
#define key_storeui 172
#define key_vstorec 173
#define key_vstores 174
#define key_vstorei 175
#define key_vstorep 176
#define key_vstorel 177
#define key_vstoreuc 178
#define key_vstoreus 179
#define key_vstoreui 180
#define key_astorec 181
#define key_astores 182
#define key_astorei 183
#define key_astorep 184
#define key_astorel 185
#define key_astoreuc 186
#define key_astoreus 187
#define key_astoreui 188
#define key_vastorec 189
#define key_vastores 190
#define key_vastorei 191
#define key_vastorep 192
#define key_vastorel 193
#define key_vastoreuc 194
#define key_vastoreus 195
#define key_vastoreui 196
#define key_hlt 197
#define key_call 198
#define key_copy 199
#define key_pcopy 200
#define key_popa 201
#define key_put 202
#define key_pushsp 203
#define key_set 204
#define key_hltr 205
#define key_incsp 206
#define key_decsp 207
#define key_exit 208
#define key_force_panic 209
#define key_pushlt 210
#define key_import 211
#define key_hidden 212
#define key_exposed 213
#define key_struct 214
#define key_uc 215
#define key_us 216
#define key_ui 217
#define key_c 218
#define key_s 219
#define key_i 220
#define key_l 221
#define key_p 222
#define key_alignof 223
#define key_sizeof 224
#define key_if 225
#define key_else 226
#define key_rcall 227
#define key_open 228
#define key_invoke 229
#define key_pushpc 230
#define key_pushcs 231

#endif
#endif

#ifdef TOC_ADD_NUMERIC_CONSTANT
#ifndef TOC_ADDED_NUMERIC_CONSTANT
#define TOC_ADDED_NUMERIC_CONSTANT

//numeric-constant
#define decimal_constant 0
#define octal_constant 1
#define binary_constant 2
#define hexadecimal_constant 3

#endif
#endif

#ifdef TOC_ADD_PUNCTUATOR
#ifndef TOC_ADDED_PUNCTUATOR
#define TOC_ADDED_PUNCTUATOR

//punctuator
#define punc_colon 0
#define punc_minus 1
#define punc_plus 2
#define punc_dot 3
#define punc_openingcbrace 4
#define punc_closingcbrace 5
#define punc_openingparen 6
#define punc_closingparen 7

#endif
#endif

#ifdef TOC_ADD_SIGN
#ifndef TOC_ADDED_SIGN
#define TOC_ADDED_SIGN

//sign
#define sign_positive 1
#define sign_negative -1

#endif
#endif

#ifdef TOC_ADD_VISIBILITY
#ifndef TOC_ADDED_VISIBILITY
#define TOC_ADDED_VISIBILITY

//visibility
#define visibility_hidden 0
#define visibility_exposed 1

#endif
#endif
