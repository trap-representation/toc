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
#define key_jmp 8
#define key_jeq 9
#define key_jgt 10
#define key_jls 11
#define key_jge 12
#define key_jle 13
#define key_jne 14
#define key_jz 15
#define key_jnz 16
#define key_addc 17
#define key_adds 18
#define key_addi 19
#define key_addp 20
#define key_addl 21
#define key_adduc 22
#define key_addus 23
#define key_addui 24
#define key_subc 25
#define key_subs 26
#define key_subi 27
#define key_subp 28
#define key_subl 29
#define key_subuc 30
#define key_subus 31
#define key_subui 32
#define key_mulc 33
#define key_muls 34
#define key_muli 35
#define key_mull 36
#define key_muluc 37
#define key_mulus 38
#define key_mului 39
#define key_divc 40
#define key_divs 41
#define key_divi 42
#define key_divl 43
#define key_divuc 44
#define key_divus 45
#define key_divui 46
#define key_remc 47
#define key_rems 48
#define key_remi 49
#define key_reml 50
#define key_remuc 51
#define key_remus 52
#define key_remui 53
#define key_lshc 54
#define key_lshs 55
#define key_lshi 56
#define key_lshl 57
#define key_lshuc 58
#define key_lshus 59
#define key_lshui 60
#define key_rshc 61
#define key_rshs 62
#define key_rshi 63
#define key_rshl 64
#define key_rshuc 65
#define key_rshus 66
#define key_rshui 67
#define key_orc 68
#define key_ors 69
#define key_ori 70
#define key_orl 71
#define key_oruc 72
#define key_orus 73
#define key_orui 74
#define key_andc 75
#define key_ands 76
#define key_andi 77
#define key_andl 78
#define key_anduc 79
#define key_andus 80
#define key_andui 81
#define key_notc 82
#define key_nots 83
#define key_noti 84
#define key_notl 85
#define key_notuc 86
#define key_notus 87
#define key_notui 88
#define key_xorc 89
#define key_xors 90
#define key_xori 91
#define key_xorl 92
#define key_xoruc 93
#define key_xorus 94
#define key_xorui 95
#define key_swap 96
#define key_pop 97
#define key_dup 98
#define key_ret 99
#define key_over 100
#define key_panic 101
#define key_write0 102
#define key_write1 103
#define key_write2 104
#define key_write3 105
#define key_write4 106
#define key_write5 107
#define key_write6 108
#define key_write7 109
#define key_vwrite0 110
#define key_vwrite1 111
#define key_vwrite2 112
#define key_vwrite3 113
#define key_vwrite4 114
#define key_vwrite5 115
#define key_vwrite6 116
#define key_vwrite7 117
#define key_get0 118
#define key_get1 119
#define key_get2 120
#define key_get3 121
#define key_get4 122
#define key_get5 123
#define key_get6 124
#define key_get7 125
#define key_vget0 126
#define key_vget1 127
#define key_vget2 128
#define key_vget3 129
#define key_vget4 130
#define key_vget5 131
#define key_vget6 132
#define key_vget7 133
#define key_loadc 134
#define key_loads 135
#define key_loadi 136
#define key_loadp 137
#define key_loadl 138
#define key_loaduc 139
#define key_loadus 140
#define key_loadui 141
#define key_vloadc 142
#define key_vloads 143
#define key_vloadi 144
#define key_vloadp 145
#define key_vloadl 146
#define key_vloaduc 147
#define key_vloadus 148
#define key_vloadui 149
#define key_aloadc 150
#define key_aloads 151
#define key_aloadi 152
#define key_aloadp 153
#define key_aloadl 154
#define key_aloaduc 155
#define key_aloadus 156
#define key_aloadui 157
#define key_valoadc 158
#define key_valoads 159
#define key_valoadi 160
#define key_valoadp 161
#define key_valoadl 162
#define key_valoaduc 163
#define key_valoadus 164
#define key_valoadui 165
#define key_storec 166
#define key_stores 167
#define key_storei 168
#define key_storep 169
#define key_storel 170
#define key_storeuc 171
#define key_storeus 172
#define key_storeui 173
#define key_vstorec 174
#define key_vstores 175
#define key_vstorei 176
#define key_vstorep 177
#define key_vstorel 178
#define key_vstoreuc 179
#define key_vstoreus 180
#define key_vstoreui 181
#define key_astorec 182
#define key_astores 183
#define key_astorei 184
#define key_astorep 185
#define key_astorel 186
#define key_astoreuc 187
#define key_astoreus 188
#define key_astoreui 189
#define key_vastorec 190
#define key_vastores 191
#define key_vastorei 192
#define key_vastorep 193
#define key_vastorel 194
#define key_vastoreuc 195
#define key_vastoreus 196
#define key_vastoreui 197
#define key_hlt 198
#define key_call 199
#define key_cleq 200
#define key_clgt 201
#define key_clls 202
#define key_clge 203
#define key_clle 204
#define key_clne 205
#define key_clz 206
#define key_clnz 207
#define key_copy 208
#define key_pcopy 209
#define key_popa 210
#define key_put 211
#define key_pushsp 212
#define key_set 213
#define key_hltr 214
#define key_nop 215
#define key_alignc 216
#define key_aligns 217
#define key_aligni 218
#define key_alignp 219
#define key_alignl 220
#define key_alignuc 221
#define key_alignus 222
#define key_alignui 223
#define key_incsp 224
#define key_decsp 225
#define key_exit 226
#define key_force_panic 227
#define key_jmp_panic 228
#define key_pushlt 229
#define key_import 230
#define key_hidden 231
#define key_exposed 232
#define key_struct 233
#define key_uc 234
#define key_us 235
#define key_ui 236
#define key_c 237
#define key_s 238
#define key_i 239
#define key_l 240
#define key_p 241
#define key_alignof 242
#define key_sizeof 243
#define key_ceq 244
#define key_cgt 245
#define key_cls 246
#define key_cge 247
#define key_cle 248
#define key_cne 249
#define key_cz 250
#define key_cnz 251

//toc-specific
#define key_open 252
#define key_invoke 253
#define key_pushpc 254
#define key_pushcs 255

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
#define punc_dollar 3
#define punc_bang 5
#define punc_dot 6

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
