/*
   Copyright (C) 2003 Paul Brossier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "utils.h"

unsigned int pos = 0; /*frames%dspblocksize*/

aubio_pitch_t *o;
fvec_t *pitch;

static int aubio_process(smpl_t **input, smpl_t **output, int nframes) {
  unsigned int i;       /*channels*/
  unsigned int j;       /*frames*/
  for (j=0;j<(unsigned)nframes;j++) {
    if(usejack) {
      for (i=0;i<channels;i++) {
        /* write input to datanew */
        fvec_write_sample(ibuf, input[i][j], i, pos);
        /* put synthnew in output */
        output[i][j] = fvec_read_sample(obuf, i, pos);
      }
    }
    /*time for fft*/
    if (pos == overlap_size-1) {         
      /* block loop */
      aubio_pitch_do (o, ibuf, pitch);
      if (fvec_read_sample(pitch, 0, 0)) {
        for (pos = 0; pos < overlap_size; pos++){
          // TODO, play sine at this freq
        }
      } else {
        fvec_zeros (obuf);
      }
      /* end of block loop */
      pos = -1; /* so it will be zero next j loop */
    }
    pos++;
  }
  return 1;
}

static void process_print (void) {
      if (!verbose && usejack) return;
      smpl_t pitch_found = fvec_read_sample(pitch, 0, 0);
      outmsg("%f %f\n",(frames) 
              *overlap_size/(float)samplerate, pitch_found);
}

int main(int argc, char **argv) {
  examples_common_init(argc,argv);

  o = new_aubio_pitch (onset_mode, buffer_size, overlap_size, channels,
          samplerate);
  pitch = new_fvec (1, channels);

  examples_common_process(aubio_process,process_print);

  del_aubio_pitch (o);
  del_fvec (pitch);

  examples_common_del();
  debug("End of program.\n");
  fflush(stderr);
  return 0;
}

