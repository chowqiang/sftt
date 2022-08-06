/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

/*
 * 1. Generate test dir in server
 * 2. Generate test dir in client
 * 3. Generate random file and dir in client test dir
 * 4. Start server
 * 5. Start client and read commands from file
 * 6. Client run commands
 * 7. Check file md5 between client random file and server gotten file
 * 8. Check dir between client random dir and server gotten dir
 * 9. Give checked results
 */
int main(int argc, char *argv[])
{
	printf("put test done!\n");
	return 0;
}
