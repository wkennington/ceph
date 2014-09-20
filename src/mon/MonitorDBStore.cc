// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
* Ceph - scalable distributed file system
*
* Copyright (C) 2012 Inktank, Inc.
*
* This is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License version 2.1, as published by the Free Software
* Foundation. See file COPYING.
*/

#include "MonitorDBStore.h"
#include "LevelDBStore.h"

void MonitorDBStore::init_options()
{
  db->init();
  if (g_conf->mon_leveldb_write_buffer_size)
    db->options.write_buffer_size = g_conf->mon_leveldb_write_buffer_size;
  if (g_conf->mon_leveldb_cache_size)
    db->options.cache_size = g_conf->mon_leveldb_cache_size;
  if (g_conf->mon_leveldb_block_size)
    db->options.block_size = g_conf->mon_leveldb_block_size;
  if (g_conf->mon_leveldb_bloom_size)
    db->options.bloom_size = g_conf->mon_leveldb_bloom_size;
  if (g_conf->mon_leveldb_compression)
    db->options.compression_enabled = g_conf->mon_leveldb_compression;
  if (g_conf->mon_leveldb_max_open_files)
    db->options.max_open_files = g_conf->mon_leveldb_max_open_files;
  if (g_conf->mon_leveldb_paranoid)
    db->options.paranoid_checks = g_conf->mon_leveldb_paranoid;
  if (g_conf->mon_leveldb_log.length())
    db->options.log_file = g_conf->mon_leveldb_log;
}
