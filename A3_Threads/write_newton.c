#include "write_newton.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <string.h>

int writenewton(void *arguments)
{   
    const write_thread_struct *thread_info = (write_thread_struct*)arguments;
    FILE *convergenceFile = thread_info -> convergenceFile;
    FILE *attractorFile = thread_info -> attractorFile;

    char *colorMap[10] = {
                "180 000 030", "000 180 030", "000 030 180", "000 190 180", "180 000 175",
                "180 255 000", "155 170 180", "070 050 000", "150 060 000", "000 150 060"
    };
    char *colorMap_convergence[50] = {
			   "005 005 005 ", "010 010 010 ", "015 015 015 ", "020 020 020 ", "025 025 025 ",
			   "030 030 030 ", "035 035 035 ", "040 040 040 ", "045 045 045 ", "050 050 050 ",
			   "056 056 056 ", "061 061 061 ", "066 066 066 ", "071 071 071 ", "076 076 076 ",
			   "081 081 081 ", "086 086 086 ", "091 091 091 ", "096 096 096 ", "100 100 100 ",
			   "107 107 107 ", "112 112 112 ", "117 117 117 ", "122 122 122 ", "127 127 127 ",
			   "132 132 132 ", "137 137 137 ", "142 142 142 ", "147 147 147 ", "153 153 153 ",
			   "158 158 158 ", "163 163 163 ", "168 168 168 ", "173 173 173 ", "178 178 178 ",
			   "183 183 183 ", "188 188 188 ", "193 193 193 ", "198 198 198 ", "204 204 204 ",
			   "209 209 209 ", "214 214 214 ", "219 219 219 ", "224 224 224 ", "229 229 229 ",
			   "234 234 234 ", "239 239 239 ", "244 244 244 ", "249 249 249 ", "255 255 255 "
    };

    extern int numb_rows;
    char attractor_color_row[12 * numb_rows], convergence_color_row[12 * numb_rows];

    struct timespec sleep_timespec;
    sleep_timespec.tv_sec = 0;
    sleep_timespec.tv_nsec = 1000; //Sleep 1 microsecond

    for(int i = 0; i < numb_rows; ++i)
    {
        while(!thread_info -> computed[i])
        {//Write only when a thread that handles the computing part has finished a row
            nanosleep(&sleep_timespec, NULL);
            continue;
        }

        char *attractor_row = thread_info -> attractor + i * numb_rows;
        char *convergence_row = thread_info -> convergence + i * numb_rows;

        for(int j = 0; j < numb_rows; ++j)
        {
            char *attractorColor = colorMap[attractor_row[j]];
            memcpy(attractor_color_row + 12*j, attractorColor, 12);

            char *convergenceColor = colorMap_convergence[convergence_row[j]];
            memcpy(convergence_color_row + 12*j, convergenceColor, 12);
        }

        attractor_color_row[12 * numb_rows - 1] = '\n';
        convergence_color_row[12 * numb_rows - 1] = '\n';

        fwrite(attractor_color_row, sizeof(char), 12*numb_rows, attractorFile);
        fwrite(convergence_color_row, sizeof(char), 12*numb_rows, convergenceFile);
    }
    
}
