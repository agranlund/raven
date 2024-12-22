/* Prototypes for functions defined in compil.c */
void CO_exec(void);
void CO_term(short);
short CO_init(void);

/* Prototypes for functions defined in libdial.c */
OBJECT *allocate(long);
OBJECT *adr_box(long);
long init_box(long,long,long);
void copy_screen(long,long,long,long,MFDB *,MFDB *);
long text_box(long,long,long,BYTE *);
long button_box(long,long,long,BYTE *,long);
long gtext_box(long,long,long,BYTE *,long,int,long);
int color_box(long,long,long,long,long);
BYTE *readstr_box(long,long);
long readbut_box(long,long);
long edit_box(long,long,long,BYTE *,BYTE *,BYTE *,long);
long draw_box(long);

/* Prototypes for functions defined in libmenu.c */
void copy_object(long *,long *);
int init_menu(long,long,long,long);
int draw_menu(void);
int delete_menu(void);
long title_menu(long);
long item_menu(long);
int check_menu(long);
int enable_menu(long);
int select_menu(long);

/* Prototypes for functions defined in libresou.c */
int rs_objstate(long,short);
void rs_objselect(long,short);
void rs_objunselect(long,short);
void rs_objxywh(long,short,GRECT *);
void rs_drawobject(long,long);
void set_clip(int,int,int,int);
char *strtime(char *);
char *strdate(char *);
int rect_point(short *,int,int);
void rect_union(short *,short *);
void rect_init(short *,int,int,int,int);
long rs_addralert(short);
long rs_addrdial(short);
long rs_addredit(long,short);
long rs_addrbutton(long,short);
int rs_drawalert(int);
void rs_drawdial(long);
void rs_erasedial(void);

/* Prototypes for functions defined in libwind.c */
void clip(short);
long open_window(long,long,long,long,long,BYTE *,BYTE *);
long close_window(long);
long clear_window(long);
long print_window(short,BYTE *);
long pos_window(short,short,short);
long size_window(long,long *,long *,long *,long *);
void close_all(void);
void draw(short,short,short,short);
long mouse(long *,long *,int);
long event(long *,long *,short *,long *,long *,long *);
