/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggTheora SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE Theora SOURCE CODE IS COPYRIGHT (C) 2002-2009                *
 * by the Xiph.Org Foundation and contributors http://www.xiph.org/ *
 *                                                                  *
 ********************************************************************

  function:
    last mod: $Id$

 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "apiwrapper.h"
#include "internal.h"
#pragma warning(disable: 4267)
 /*This is more or less the same as strncasecmp, but that doesn't exist
 everywhere, and this is a fairly trivial function, so we include it.
 Note: We take advantage of the fact that we know _n is less than or equal to
 the length of at least one of the strings.*/
static int oc_tagcompare(const char *_s1, const char *_s2, int _n) {
	int c;
	for (c = 0;c<_n;c++) {
		if (toupper(_s1[c]) != toupper(_s2[c]))return !0;
	}
	return _s1[c] != '=';
}

void th_info_init(th_info *_info) {
	memset(_info, 0, sizeof(*_info));
	_info->version_major = TH_VERSION_MAJOR;
	_info->version_minor = TH_VERSION_MINOR;
	_info->version_subminor = TH_VERSION_SUB;
	_info->keyframe_granule_shift = 6;
}

void th_info_clear(th_info *_info) {
	memset(_info, 0, sizeof(*_info));
}

void th_comment_init(th_comment *_tc) {
	memset(_tc, 0, sizeof(*_tc));
}

void th_comment_add(th_comment *_tc, const char *_comment) {
	char **user_comments;
	int   *comment_lengths;
	int    comment_len;
	user_comments = _ogg_realloc(_tc->user_comments,
		(_tc->comments + 2) * sizeof(*_tc->user_comments));
	if (user_comments == NULL)return;
	_tc->user_comments = user_comments;
	comment_lengths = _ogg_realloc(_tc->comment_lengths,
		(_tc->comments + 2) * sizeof(*_tc->comment_lengths));
	if (comment_lengths == NULL)return;
	_tc->comment_lengths = comment_lengths;
	comment_len = strlen(_comment);
	comment_lengths[_tc->comments] = comment_len;
	user_comments[_tc->comments] = _ogg_malloc(comment_len + 1);
	if (user_comments[_tc->comments] == NULL)return;
	memcpy(_tc->user_comments[_tc->comments], _comment, comment_len + 1);
	_tc->comments++;
	_tc->user_comments[_tc->comments] = NULL;
}

void th_comment_add_tag(th_comment *_tc, const char *_tag, const char *_val) {
	char *comment;
	int   tag_len;
	int   val_len;
	tag_len = strlen(_tag);
	val_len = strlen(_val);
	/*+2 for '=' and '\0'.*/
	comment = _ogg_malloc(tag_len + val_len + 2);
	if (comment == NULL)return;
	memcpy(comment, _tag, tag_len);
	comment[tag_len] = '=';
	memcpy(comment + tag_len + 1, _val, val_len + 1);
	th_comment_add(_tc, comment);
	_ogg_free(comment);
}

char *th_comment_query(th_comment *_tc, const char *_tag, int _count) {
	long i;
	int  found;
	int  tag_len;
	tag_len = strlen(_tag);
	found = 0;
	for (i = 0;i<_tc->comments;i++) {
		if (!oc_tagcompare(_tc->user_comments[i], _tag, tag_len)) {
			/*We return a pointer to the data, not a copy.*/
			if (_count == found++)return _tc->user_comments[i] + tag_len + 1;
		}
	}
	/*Didn't find anything.*/
	return NULL;
}

int th_comment_query_count(th_comment *_tc, const char *_tag) {
	long i;
	int  tag_len;
	int  count;
	tag_len = strlen(_tag);
	count = 0;
	for (i = 0;i<_tc->comments;i++) {
		if (!oc_tagcompare(_tc->user_comments[i], _tag, tag_len))count++;
	}
	return count;
}

void th_comment_clear(th_comment *_tc) {
	if (_tc != NULL) {
		long i;
		for (i = 0;i<_tc->comments;i++)_ogg_free(_tc->user_comments[i]);
		_ogg_free(_tc->user_comments);
		_ogg_free(_tc->comment_lengths);
		_ogg_free(_tc->vendor);
		memset(_tc, 0, sizeof(*_tc));
	}
}


const char *theora_version_string(void){
  return th_version_string();
}

ogg_uint32_t theora_version_number(void){
  return th_version_number();
}

void theora_info_init(theora_info *_ci){
  memset(_ci,0,sizeof(*_ci));
}
 
void theora_info_clear(theora_info *c){
  th_api_wrapper *api;
  api=(th_api_wrapper *)c->codec_setup;
  memset(c,0,sizeof(*c));
  if(api!=NULL){
    if(api->clear!=NULL)(*api->clear)(api);
    _ogg_free(api);
  }
}

void theora_clear(theora_state *_th){
  /*Provide compatibility with mixed encoder and decoder shared lib versions.*/
  if(_th->internal_decode!=NULL){
    (*((oc_state_dispatch_vtable *)_th->internal_decode)->clear)(_th);
  }
  if(_th->internal_encode!=NULL){
    (*((oc_state_dispatch_vtable *)_th->internal_encode)->clear)(_th);
  }
  if(_th->i!=NULL)theora_info_clear(_th->i);
  memset(_th,0,sizeof(*_th));
}

int theora_control(theora_state *_th,int _req,void *_buf,size_t _buf_sz){
  /*Provide compatibility with mixed encoder and decoder shared lib versions.*/
  if(_th->internal_decode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_decode)->control)(_th,
     _req,_buf,_buf_sz);
  }
  else if(_th->internal_encode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_encode)->control)(_th,
     _req,_buf,_buf_sz);
  }
  else return TH_EINVAL;
}

ogg_int64_t theora_granule_frame(theora_state *_th,ogg_int64_t _gp){
  /*Provide compatibility with mixed encoder and decoder shared lib versions.*/
  if(_th->internal_decode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_decode)->granule_frame)(
     _th,_gp);
  }
  else if(_th->internal_encode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_encode)->granule_frame)(
     _th,_gp);
  }
  else return -1;
}

double theora_granule_time(theora_state *_th, ogg_int64_t _gp){
  /*Provide compatibility with mixed encoder and decoder shared lib versions.*/
  if(_th->internal_decode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_decode)->granule_time)(
     _th,_gp);
  }
  else if(_th->internal_encode!=NULL){
    return (*((oc_state_dispatch_vtable *)_th->internal_encode)->granule_time)(
     _th,_gp);
  }
  else return -1;
}

void oc_theora_info2th_info(th_info *_info,const theora_info *_ci){
  _info->version_major=_ci->version_major;
  _info->version_minor=_ci->version_minor;
  _info->version_subminor=_ci->version_subminor;
  _info->frame_width=_ci->width;
  _info->frame_height=_ci->height;
  _info->pic_width=_ci->frame_width;
  _info->pic_height=_ci->frame_height;
  _info->pic_x=_ci->offset_x;
  _info->pic_y=_ci->offset_y;
  _info->fps_numerator=_ci->fps_numerator;
  _info->fps_denominator=_ci->fps_denominator;
  _info->aspect_numerator=_ci->aspect_numerator;
  _info->aspect_denominator=_ci->aspect_denominator;
  switch(_ci->colorspace){
    case OC_CS_ITU_REC_470M:_info->colorspace=TH_CS_ITU_REC_470M;break;
    case OC_CS_ITU_REC_470BG:_info->colorspace=TH_CS_ITU_REC_470BG;break;
    default:_info->colorspace=TH_CS_UNSPECIFIED;break;
  }
  switch(_ci->pixelformat){
    case OC_PF_420:_info->pixel_fmt=TH_PF_420;break;
    case OC_PF_422:_info->pixel_fmt=TH_PF_422;break;
    case OC_PF_444:_info->pixel_fmt=TH_PF_444;break;
    default:_info->pixel_fmt=TH_PF_RSVD;
  }
  _info->target_bitrate=_ci->target_bitrate;
  _info->quality=_ci->quality;
  _info->keyframe_granule_shift=_ci->keyframe_frequency_force>0?
   OC_MINI(31,oc_ilog(_ci->keyframe_frequency_force-1)):0;
}

int theora_packet_isheader(ogg_packet *_op){
  return th_packet_isheader(_op);
}

int theora_packet_iskeyframe(ogg_packet *_op){
  return th_packet_iskeyframe(_op);
}

int theora_granule_shift(theora_info *_ci){
  /*This breaks when keyframe_frequency_force is not positive or is larger than
     2**31 (if your int is more than 32 bits), but that's what the original
     function does.*/
  return oc_ilog(_ci->keyframe_frequency_force-1);
}

void theora_comment_init(theora_comment *_tc){
  th_comment_init((th_comment *)_tc);
}

char *theora_comment_query(theora_comment *_tc,char *_tag,int _count){
  return th_comment_query((th_comment *)_tc,_tag,_count);
}

int theora_comment_query_count(theora_comment *_tc,char *_tag){
  return th_comment_query_count((th_comment *)_tc,_tag);
}

void theora_comment_clear(theora_comment *_tc){
  th_comment_clear((th_comment *)_tc);
}

void theora_comment_add(theora_comment *_tc,char *_comment){
  th_comment_add((th_comment *)_tc,_comment);
}

void theora_comment_add_tag(theora_comment *_tc, char *_tag, char *_value){
  th_comment_add_tag((th_comment *)_tc,_tag,_value);
}
