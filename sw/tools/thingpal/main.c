/*-------------------------------------------------------------------------------
 * thingpal
 * (c)2024 Anders Granlund
 *
 * Converts nova_col palette to other formats.
 *
 * in:   c:\nova_col.pal
 * out1: thingimg.pal     (ThingImg palette)
 * out2: thingimg.act     (Photoshop palette)
 * out2: thingimx.act     (Photoshop palette, ignore first 16)
 *
 *-------------------------------------------------------------------------------
 *
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>

int main()
{
	FILE *in, *out1, *out2, *out3;
	unsigned char b[6],c[3];
	int ret, i;

	const char* fname_out1 = "thingimg.pal";
	const char* fname_out2 = "thingimg.act";
	const char* fname_out3 = "thingimx.act";
	char fname_in[128] = "c:\\nova_col.inf";

	/* open files */
	ret = -1;
	printf("Opening %s\n", fname_in);
	in = fopen(fname_in, "rb");
	if (in == 0) {
		printf("Failed opening '%s'\n", fname_in);
		goto done;		
	}
	out1 = fopen(fname_out1, "wb");
	if (out1 == 0) {
		printf("Failed opening '%s'\n", fname_out1);
		goto done;
	}
	out2 = fopen(fname_out2, "wb");
	if (out2 == 0) {
		printf("Failed opening '%s'\n", fname_out2);
		goto done;

	}
	out3 = fopen(fname_out3, "wb");
	if (out2 == 0) {
		printf("Failed opening '%s'\n", fname_out3);
		goto done;

	}

	/* convert palette */
	fseek(in, 18, SEEK_SET);	
	for (i = 0; i < 256; i++)
	{
		fread(b, 1, 6, in);
		fwrite(b, 1, 6, out1);
		c[0] = (unsigned char) (((unsigned long)((b[0]<<8)+b[1]) * 255) / 1000);
		c[1] = (unsigned char) (((unsigned long)((b[2]<<8)+b[3]) * 255) / 1000);
		c[2] = (unsigned char) (((unsigned long)((b[4]<<8)+b[5]) * 255) / 1000);


		fwrite(c, 1, 3, out2);
		if (i < 16) {
			c[0] = 255;
			c[1] = 0;
			c[2] = 255;
		}
		fwrite(c, 1, 3, out3);
	}
	
	/* success */
	ret = 0;

done:
	/* cleanup */

	if (in)
		fclose(in);
	if (out1)
		fclose(out1);
	if (out2)
		fclose(out2);
	if (out3)
		fclose(out3);

	if (ret == 0) {
		printf("Wrote %s\n", fname_out1);
		printf("Wrote %s\n", fname_out2);
		printf("Wrote %s\n", fname_out3);
	}

	return ret;
}


