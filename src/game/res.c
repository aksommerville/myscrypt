/* res.c
 * Manages resources.
 */
 
#include "myscrypt.h"

/* Map.
 * We'll preread all the positions and store them privately by position.
 */
 
static int rcv_map(int rid,const void *src,int srcc) {
  struct map_res res;
  if (map_res_decode(&res,src,srcc)<0) return -1;
  if ((res.w!=NS_sys_mapw)||(res.h!=NS_sys_maph)) {
    fprintf(stderr,"map:%d size %d,%d, expected %d,%d\n",rid,res.w,res.h,NS_sys_mapw,NS_sys_maph);
    return -2;
  }
  int lat=0,lng=0;
  struct cmdlist_reader reader;
  if (cmdlist_reader_init(&reader,res.cmd,res.cmdc)<0) return -1;
  struct cmdlist_entry cmd;
  while (cmdlist_reader_next(&cmd,&reader)>0) {
    switch (cmd.opcode) {
      case CMD_map_position: {
          lng=cmd.arg[0];
          lat=cmd.arg[1];
        } break;
    }
  }
  if ((lng<LNG_MIN)||(lng>LNG_MAX)||(lat<LAT_MIN)||(lat>LAT_MAX)) {
    fprintf(stderr,"map:%d position %d,%d outside world %d..%d,%d..%d\n",rid,lng,lat,LNG_MIN,LNG_MAX,LAT_MIN,LAT_MAX);
    return -2;
  }
  int x=lng-LNG_MIN;
  int y=lat-LAT_MIN;
  int p=y*(LNG_MAX-LNG_MIN+1)+x;
  struct map *map=g.mapv+p;
  map->rid=rid;
  map->lng=lng;
  map->lat=lat;
  map->v=res.v;
  map->cmd=res.cmd;
  map->cmdc=res.cmdc;
  return 0;
}

struct map *map_by_id(int rid) {
  int p=res_search(EGG_TID_map,rid);
  if (p<0) return 0;
  const struct rom_entry *res=g.resv+p;
  struct map_res map_res;
  if (map_res_decode(&map_res,res->v,res->c)<0) return 0;
  struct cmdlist_reader reader;
  if (cmdlist_reader_init(&reader,map_res.cmd,map_res.cmdc)<0) return 0;
  struct cmdlist_entry cmd;
  while (cmdlist_reader_next(&cmd,&reader)>0) {
    switch (cmd.opcode) {
      case CMD_map_position: {
          return map_by_position(cmd.arg[0],cmd.arg[1]);
        } break;
    }
  }
  return 0;
}

struct map *map_by_position(int lng,int lat) {
  if ((lng<LNG_MIN)||(lng>LNG_MAX)||(lat<LAT_MIN)||(lat>LAT_MAX)) return 0;
  int p=(lat-LAT_MIN)*(LNG_MAX-LNG_MIN+1)+lng-LNG_MIN;
  struct map *map=g.mapv+p;
  if (!map->v) return 0; // Don't return unset maps, even if the coords are valid.
  return map;
}

/* Tilesheet.
 * There is only one and it only has one table.
 */
 
static int rcv_tilesheet(int rid,const void *src,int srcc) {
  struct tilesheet_reader reader;
  if (tilesheet_reader_init(&reader,src,srcc)<0) return -1;
  struct tilesheet_entry entry;
  while (tilesheet_reader_next(&entry,&reader)>0) {
    memcpy(g.physics+entry.tileid,entry.v,entry.c);
  }
  return 0;
}

/* Init.
 */
 
int res_init() {

  // Acquire the encoded ROM.
  g.romc=egg_rom_get(0,0);
  if (!(g.rom=malloc(g.romc))) return -1;
  egg_rom_get(g.rom,g.romc);
  
  // Split out the resource TOC.
  struct rom_reader reader;
  if (rom_reader_init(&reader,g.rom,g.romc)<0) return -1;
  struct rom_entry res;
  while (rom_reader_next(&res,&reader)>0) {
    if (g.resc>=g.resa) {
      int na=g.resa+64;
      if (na>INT_MAX/sizeof(struct rom_entry)) return -1;
      void *nv=realloc(g.resv,sizeof(struct rom_entry)*na);
      if (!nv) return -1;
      g.resv=nv;
      g.resa=na;
    }
    g.resv[g.resc++]=res;
  }
  
  // Take any type-specific actions.
  const struct rom_entry *resp=g.resv;
  int i=g.resc;
  for (;i-->0;resp++) {
    switch (resp->tid) {
      case EGG_TID_map: if (rcv_map(resp->rid,resp->v,resp->c)<0) return -1; break;
      case EGG_TID_tilesheet: if (rcv_tilesheet(resp->rid,resp->v,resp->c)<0) return -1; break;
    }
  }
  
  return 0;
}

/* Get resource.
 */
 
int res_search(int tid,int rid) {
  int lo=0,hi=g.resc;
  while (lo<hi) {
    int ck=(lo+hi)>>1;
    const struct rom_entry *res=g.resv+ck;
         if (tid<res->tid) hi=ck;
    else if (tid>res->tid) lo=ck+1;
    else if (rid<res->rid) hi=ck;
    else if (rid>res->rid) lo=ck+1;
    else return ck;
  }
  return -lo-1;
}

int res_get(void *dstpp,int tid,int rid) {
  int p=res_search(tid,rid);
  if (p<0) return 0;
  const struct rom_entry *res=g.resv+p;
  *(const void**)dstpp=res->v;
  return res->c;
}
