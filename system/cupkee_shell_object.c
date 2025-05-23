/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#include "cupkee.h"
#include "cupkee_shell_util.h"

val_t cupkee_shell_object_create(env_t *env, void *entry)
{
    (void) env;
    if (entry) {
        return val_mk_foreign((intptr_t)entry);
    } else {
        return VAL_UNDEFINED;
    }
}

void *cupkee_shell_object_entry (val_t *v)
{
    if (val_is_foreign(v)) {
        return (void*) val_2_intptr(v);
    }
    return NULL;
}

