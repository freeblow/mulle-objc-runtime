//
//  mulle_objc_retain_release.c
//  mulle-objc
//
//  Created by Nat! on 16/11/14.
//  Copyright (c) 2014 Nat! - Mulle kybernetiK.
//  Copyright (c) 2014 Codeon GmbH.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//

#include "mulle_objc_retain_release.h"

#include "mulle_objc_call.h"
#include "mulle_objc_object_convenience.h"



uintptr_t  __mulle_objc_object_get_retaincount( void *obj)
{
   struct _mulle_objc_objectheader    *header;
   intptr_t                            retaincount_1;

   header        = _mulle_objc_object_get_objectheader( obj);
   retaincount_1 = _mulle_objc_objectheader_get_retaincount_1( header);
   if( retaincount_1 == MULLE_OBJC_NEVER_RELEASE)
      return( MULLE_OBJC_NEVER_RELEASE);
   if( retaincount_1 == -1)
      return( 0);
   if( retaincount_1 < 0)
      return( retaincount_1 - INTPTR_MIN);
   return( retaincount_1 + 1);
}

/* ideally, we enter with retainCount == 0
   call finalize, afterwards retainCount is still 0
   then we call dealloc

   but finalize may up the retainCount and we don't want to
   call it again, nor do we want to call dealloc

   so we add INTPTR_MIN to the retainCount and only dealloc
   when we reach INTPTR_MIN again
 */

void  _mulle_objc_object_try_finalize_try_dealloc( void *obj)
{
   struct _mulle_objc_objectheader     *header;
   volatile intptr_t                   retaincount_1;  // volatile vooodo ?

   // when we enter this, we are single threaded if -1
   // but we are not when we have been finalized

   header        = _mulle_objc_object_get_objectheader( obj);
   retaincount_1 = (intptr_t) _mulle_atomic_pointer_read( &header->_retaincount_1);

   if( retaincount_1 == -1)  // yes ? :: single threaded(!)
   {
      // but potentially not anymore after that
      retaincount_1 += INTPTR_MIN + 1;
      _mulle_atomic_pointer_nonatomic_write( &header->_retaincount_1, (void *) retaincount_1);

      _mulle_objc_object_finalize( obj);

      // reread
      retaincount_1 = (intptr_t) _mulle_atomic_pointer_read( &header->_retaincount_1);
   }

   if( retaincount_1 == INTPTR_MIN)    // yes ? :: single threaded(!)
   {
      // set it back to be nice
      _mulle_atomic_pointer_nonatomic_write( &header->_retaincount_1, (void *) -1);
      _mulle_objc_object_dealloc( obj);
   }
}


//
// this is the "user" method to finalize
//
void  _mulle_objc_object_perform_finalize( void *obj)
{
   struct _mulle_objc_objectheader     *header;
   volatile intptr_t                    retaincount_1;  // volatile vooodo ?
   intptr_t                             new_retaincount_1;

   header = _mulle_objc_object_get_objectheader( obj);
   do
   {
      retaincount_1 = (intptr_t) _mulle_atomic_pointer_read( &header->_retaincount_1);
      if( retaincount_1 < 0)
         return;     // already finalized

      new_retaincount_1 = retaincount_1 + INTPTR_MIN + 1;
   }
   while( ! _mulle_atomic_pointer_compare_and_swap( &header->_retaincount_1, (void *) new_retaincount_1,  (void *) retaincount_1));

   _mulle_objc_object_finalize( obj);
}


void   _mulle_objc_objects_call_retain( void **objects, size_t n)
{
   void   **sentinel;
   void   *p;

   // assume compiler can do unrolling
   sentinel = &objects[ n];

   while( objects < sentinel)
   {
      p = *objects++;
      if( p)
         _mulle_objc_object_retain( p);
   }
}


void   _mulle_objc_objects_call_release( void **objects, size_t n)
{
   void   **sentinel;
   void   *p;

   // assume compiler can do unrolling
   sentinel = &objects[ n];

   while( objects < sentinel)
   {
      p = *objects++;
      if( p)
         _mulle_objc_object_release( p);
   }
}


void   _mulle_objc_objects_call_release_and_zero( void **objects, size_t n)
{
   void   **sentinel;
   void   *p;

   // assume compiler can do unrolling
   sentinel = &objects[ n];

   while( objects < sentinel)
   {
      p = *objects++;
      if( p)
      {
         objects[ -1] = 0;
         _mulle_objc_object_release( p);
      }
   }
}

