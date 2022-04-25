/* GIMP RGB C-Source image dump (F1.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[34 * 24 * 2 + 1];
} f1 = {
  34, 24, 2,
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377"
  "\000\000\000\000\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377"
  "G\377\000\000\000\000\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377G\377"
  "G\377\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377"
  "G\377G\377G\377G\377G\377G\377\000\000\000\000\377\377\377\377\377\377\377\377\377"
  "\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000\377\377\377"
  "\377\377\377\377\377\000\000\000\000\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G"
  "\377\000\000\000\000\377\377\377\377\377\377\377\377\000\000\000\000\377\377\377\377\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377\000\000"
  "\000\000G\377G\377G\377G\377G\377G\377\000\000\000\000\377\377\377\377\377\377\377\377"
  "\000\000\000\000\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G"
  "\377G\377G\377G\377G\377\000\000\000\000G\377G\377G\377G\377G\377G\377\000\000\000\000\377"
  "\377\377\377\377\377\377\377\000\000\000\000\377\377\377\377\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000G\377"
  "G\377G\377G\377G\377G\377\000\000\000\000\377\377\377\377\377\377\377\377\377\377"
  "\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377"
  "G\377G\377G\377\000\000\000\000G\377G\377G\377G\377G\377G\377\000\000\000\000\377\377\377"
  "\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377"
  "G\377G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000G\377G\377G\377G\377"
  "G\377G\377G\377G\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000G\377G\377G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000G\377G\377"
  "G\377G\377G\377G\377G\377G\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377\000\000\000\000G\377G\377G\377"
  "G\377G\377G\377G\377G\377\000\000\000\000\300\374\300\374\300\374\300\374\300\374"
  "\300\374\300\374\300\374\300\374\300\374\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377"
  "G\377G\377G\377G\377\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377\000\000"
  "\000\000\300\374\300\374\300\374\300\374\300\374\300\374\300\374\300\374\300"
  "\374\300\374\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377"
  "G\377G\377G\377G\377G\377\000\000\000\000\300\374\300\374\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377"
  "G\377G\377G\377G\377G\377G\377\000\000\000\000\300\374\300\374\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377"
  "G\377G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000\300\374\300\374\300"
  "\374\300\374\300\374\300\374\300\374\300\374\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000G\377G\377G\377G\377G\377G\377G\377G\377G\377"
  "G\377\000\000\000\000\300\374\300\374\300\374\300\374\300\374\300\374\300\374\300"
  "\374\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000G\377G\377G\377G\377G\377G\377G\377G\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000",
};
