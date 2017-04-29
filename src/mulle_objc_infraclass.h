//
//  mulle_objc_infraclass.h
//  mulle-objc
//
//  Created by Nat! on 17/04/07
//  Copyright (c) 2017 Nat! - Mulle kybernetiK.
//  Copyright (c) 2017 Codeon GmbH.
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

#ifndef mulle_objc_infraclass_h__
#define mulle_objc_infraclass_h__

#include "mulle_objc_class_struct.h"
#include "mulle_objc_atomicpointer.h"
#include "mulle_objc_walktypes.h"

#include <mulle_concurrent/mulle_concurrent.h>
#include <assert.h>


struct _mulle_objc_ivar;
struct _mulle_objc_ivarlist;
struct _mulle_objc_property;
struct _mulle_objc_method;


// 0x0001 - 0x0080        - mulle-objc
// 0x0100 - 0x8000        - Foundation
// 0x00010000 - 0x80000   - User

enum _mulle_objc_infraclass_state
{
   MULLE_OBJC_INFRA_CACHE_INITIALIZED  = MULLE_OBJC_CACHE_INITIALIZED,
   MULLE_OBJC_INFRA_ALWAYS_EMPTY_CACHE = MULLE_OBJC_ALWAYS_EMPTY_CACHE,

   MULLE_OBJC_INFRA_WARN_PROTOCOL      = 0x4,
   MULLE_OBJC_INFRA_IS_PROTOCOLCLASS   = 0x8,

   MULLE_OBJC_INFRA_FOUNDATION_BIT0    = 0x00100,
   MULLE_OBJC_INFRA_FOUNDATION_BIT15   = 0x08000,

   MULLE_OBJC_INFRA_USER_BIT0          = 0x000010000
   //   MULLE_OBJC_USER_BIT15         = 0x800000000
};


struct _mulle_objc_infraclass
{
   struct _mulle_objc_class                 base;

   mulle_objc_hash_t                        ivarhash;
   mulle_atomic_pointer_t                   taggedpointerindex;
   mulle_atomic_pointer_t                   coderversion; // for NSCoder

   struct mulle_concurrent_pointerarray     ivarlists;
   struct mulle_concurrent_pointerarray     propertylists;

   struct mulle_concurrent_hashmap          cvars;
   union _mulle_objc_atomicobjectpointer_t  placeholder;
   union _mulle_objc_atomicobjectpointer_t  auxplaceholder;

};


void    _mulle_objc_infraclass_plusinit( struct _mulle_objc_infraclass *infra,
                                         struct mulle_allocator *allocator);
void    _mulle_objc_infraclass_plusdone( struct _mulle_objc_infraclass *infra);



static inline struct _mulle_objc_class   *_mulle_objc_infraclass_as_class( struct _mulle_objc_infraclass *infra)
{
   return( &infra->base);
}


static inline struct _mulle_objc_infraclass   *_mulle_objc_class_as_infraclass( struct _mulle_objc_class *cls)
{
   assert( _mulle_objc_class_is_infraclass( cls));

   return( (struct _mulle_objc_infraclass *) cls);
}


# pragma mark - conveniences

static inline struct _mulle_objc_runtime   *_mulle_objc_infraclass_get_runtime( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.runtime);
}


static inline mulle_objc_classid_t   _mulle_objc_infraclass_get_classid( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.classid);
}


static inline char   *_mulle_objc_infraclass_get_name( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.name);
}


static inline struct _mulle_objc_infraclass   *_mulle_objc_infraclass_get_superclass( struct _mulle_objc_infraclass *infra)
{
   return( (struct _mulle_objc_infraclass *) infra->base.superclass);
}


static inline struct _mulle_objc_infraclass   *mulle_objc_infraclass_get_superclass( struct _mulle_objc_infraclass *infra)
{
   return( infra ?  (struct _mulle_objc_infraclass *) infra->base.superclass : NULL);
}


static inline unsigned int   _mulle_objc_infraclass_get_inheritance( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.inheritance);
}


static inline void   _mulle_objc_infraclass_set_inheritance( struct _mulle_objc_infraclass *infra, unsigned int inheritance)
{
   assert( (unsigned short) inheritance == inheritance);

   infra->base.inheritance = (unsigned short) inheritance;
}


static inline int   _mulle_objc_infraclass_set_state_bit( struct _mulle_objc_infraclass *infra,
                                                          enum _mulle_objc_infraclass_state bit)
{
   return( _mulle_objc_class_set_state_bit( &infra->base, bit));
}


static inline unsigned int   _mulle_objc_infraclass_get_state_bit( struct _mulle_objc_infraclass *infra,
                                                                   enum _mulle_objc_infraclass_state bit)
{
   return( _mulle_objc_class_get_state_bit( &infra->base, bit));
}


static inline void   mulle_objc_infraclass_unfailing_add_methodlist( struct _mulle_objc_infraclass *infra,
                                                                     struct _mulle_objc_methodlist *list)
{
   extern void   mulle_objc_class_unfailing_add_methodlist( struct _mulle_objc_class *cls,
                                                            struct _mulle_objc_methodlist *list);

   mulle_objc_class_unfailing_add_methodlist( &infra->base, list);
}


static inline size_t   _mulle_objc_infraclass_get_allocationsize( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.allocationsize);
}


static inline size_t   _mulle_objc_infraclass_get_instancesize( struct _mulle_objc_infraclass *infra)
{
   return( infra->base.allocationsize - sizeof( struct _mulle_objc_objectheader));
}


static inline struct _mulle_objc_method  *
    mulle_objc_infraclass_search_method( struct _mulle_objc_infraclass *infra,
                                         mulle_objc_methodid_t methodid)
{
   extern struct _mulle_objc_method   *mulle_objc_class_search_method( struct _mulle_objc_class *cls,
                                                                       mulle_objc_methodid_t methodid);

   return( mulle_objc_class_search_method( &infra->base, methodid));
}


static inline struct _mulle_objc_method   *
    _mulle_objc_infraclass_search_method( struct _mulle_objc_infraclass *infra,
                                          mulle_objc_methodid_t methodid,
                                          struct _mulle_objc_method *previous,
                                          void *owner,
                                          unsigned int inheritance)
{
  extern struct _mulle_objc_method   *_mulle_objc_class_search_method( struct _mulle_objc_class *cls,
                                                              mulle_objc_methodid_t methodid,
                                                              struct _mulle_objc_method *previous,
                                                              void *owner,
                                                              unsigned int inheritance);

   return( _mulle_objc_class_search_method( &infra->base,
                                            methodid,
                                            previous,
                                            owner,
                                            inheritance));
}



#pragma mark - petty accessors

//
// the placeholder is used for +instantiate in foundation
//
static inline struct _mulle_objc_object *   _mulle_objc_infraclass_get_placeholder( struct _mulle_objc_infraclass *infra)
{
   return( _mulle_atomic_pointer_read( &infra->placeholder.pointer));
}


// 1: it has worked, 0: someone else was faster
static inline int  _mulle_objc_infraclass_set_placeholder( struct _mulle_objc_infraclass *infra,
                                                         struct _mulle_objc_object *obj)
{
   return( _mulle_atomic_pointer_compare_and_swap( &infra->placeholder.pointer, obj, NULL));
}


static inline struct _mulle_objc_object *   _mulle_objc_infraclass_get_auxplaceholder( struct _mulle_objc_infraclass *infra)
{
   return( _mulle_atomic_pointer_read( &infra->auxplaceholder.pointer));
}


// 1: it has worked, 0: someone else was faster
static inline int  _mulle_objc_infraclass_set_auxplaceholder( struct _mulle_objc_infraclass *infra,
                                                            struct _mulle_objc_object *obj)
{
   return( _mulle_atomic_pointer_compare_and_swap( &infra->auxplaceholder.pointer, obj, NULL));
}

//
// version is kept in the infraclass
//
static inline uintptr_t   _mulle_objc_infraclass_get_coderversion( struct _mulle_objc_infraclass *infra)
{
   return( (uintptr_t) _mulle_atomic_pointer_read( &infra->coderversion));
}


//
// version is kept in the infraclass
//
static inline unsigned int   _mulle_objc_infraclass_get_taggedpointerindex( struct _mulle_objc_infraclass *infra)
{
   return( (unsigned int) (uintptr_t) _mulle_atomic_pointer_read( &infra->taggedpointerindex));
}


// 1: it has worked, 0: someone else was faster, can only be set once
static inline int  _mulle_objc_infraclass_set_coderversion( struct _mulle_objc_infraclass *infra,
                                                           uintptr_t value)
{
   return( _mulle_atomic_pointer_compare_and_swap( &infra->coderversion, (void *) value, NULL));
}


// 1: it has worked, 0: someone else was faster
static inline int   _mulle_objc_infraclass_set_taggedpointerindex( struct _mulle_objc_infraclass *infra,
                                                     unsigned int value)
{
   return( _mulle_atomic_pointer_compare_and_swap( &infra->taggedpointerindex, (void *) (uintptr_t) value, NULL));
}


static inline mulle_objc_hash_t   _mulle_objc_infraclass_get_ivarhash( struct _mulle_objc_infraclass *infra)
{
   return( infra->ivarhash);
}


static inline void   _mulle_objc_infraclass_set_ivarhash( struct _mulle_objc_infraclass *infra,
                                                          mulle_objc_hash_t ivarhash)
{
   infra->ivarhash = ivarhash;
}



static inline int   _mulle_objc_infraclass_is_taggedpointerclass( struct _mulle_objc_infraclass *infra)
{
   return( _mulle_atomic_pointer_read( &infra->taggedpointerindex) != 0);
}


# pragma mark - sanity check

int   mulle_objc_infraclass_is_sane( struct _mulle_objc_infraclass *infra);



# pragma mark - class variables

static inline void   *_mulle_objc_infraclass_get_cvar( struct _mulle_objc_infraclass *infra, void *key)
{
   return( _mulle_concurrent_hashmap_lookup( &infra->cvars, (intptr_t) key));
}


static inline int   _mulle_objc_infraclass_set_cvar( struct _mulle_objc_infraclass *infra, void *key, void *value)
{
   return( _mulle_concurrent_hashmap_insert( &infra->cvars, (intptr_t) key, value));
}


static inline int   _mulle_objc_infraclass_remove_cvar( struct _mulle_objc_infraclass *infra, void *key, void *value)
{
   return( _mulle_concurrent_hashmap_remove( &infra->cvars, (intptr_t) key, value));
}


static inline struct mulle_concurrent_hashmapenumerator    _mulle_objc_infraclass_enumerate_cvars( struct _mulle_objc_infraclass *infra)
{
   return( mulle_concurrent_hashmap_enumerate( &infra->cvars));
}



# pragma mark - properties

int   mulle_objc_infraclass_add_propertylist( struct _mulle_objc_infraclass *infra,
                                              struct _mulle_objc_propertylist *list);

void   mulle_objc_infraclass_unfailing_add_propertylist( struct _mulle_objc_infraclass *infra,
                                                         struct _mulle_objc_propertylist *list);

struct _mulle_objc_property   *_mulle_objc_infraclass_search_property( struct _mulle_objc_infraclass *infra,
                                                                     mulle_objc_propertyid_t propertyid);

struct _mulle_objc_property  *mulle_objc_infraclass_search_property( struct _mulle_objc_infraclass *infra,
                                                                   mulle_objc_propertyid_t propertyid);


# pragma mark - ivar lists

int   mulle_objc_infraclass_add_ivarlist( struct _mulle_objc_infraclass *infra,
                                          struct _mulle_objc_ivarlist *list);

void   mulle_objc_infraclass_unfailing_add_ivarlist( struct _mulle_objc_infraclass *infra,
                                                struct _mulle_objc_ivarlist *list);


# pragma mark - ivars

struct _mulle_objc_ivar   *_mulle_objc_infraclass_search_ivar( struct _mulle_objc_infraclass *infra,
                                                                mulle_objc_ivarid_t ivarid);

struct _mulle_objc_ivar  *mulle_objc_infraclass_search_ivar( struct _mulle_objc_infraclass *infra,
                                                            mulle_objc_ivarid_t ivarid);


#pragma mark - walkers


int   _mulle_objc_infraclass_walk_ivars( struct _mulle_objc_infraclass *cls, unsigned int inheritance , int (*f)( struct _mulle_objc_ivar *, struct _mulle_objc_infraclass *, void *), void *userinfo);
int   _mulle_objc_infraclass_walk_properties( struct _mulle_objc_infraclass *infra, unsigned int inheritance , int (*f)( struct _mulle_objc_property *, struct _mulle_objc_infraclass *, void *), void *userinfo);


mulle_objc_walkcommand_t
   mulle_objc_infraclass_walk( struct _mulle_objc_infraclass   *infra,
                               enum mulle_objc_walkpointertype_t  type,
                               mulle_objc_walkcallback_t   callback,
                               void *parent,
                               void *userinfo);
#pragma mark - some other methods, that are only defined on infraclass

int    mulle_objc_infraclass_is_protocolclass( struct _mulle_objc_infraclass *infra);

#endif /* mulle_objc_infraclass_h */