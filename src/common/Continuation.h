// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2014 Red Hat
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "include/Context.h"
#include <set>

class Continuation {
  bool parallel_mode;
  bool parallel_failed;
  std::set<int> parallel_stages_in_flight;
  int serial_stage;
  int rval;
  Context *on_finish;
  class Callback : public Context {
    Continuation *continuation;
    int stage_to_activate;
  public:
    Callback(Continuation *c, int stage) :
      continuation(c),
      stage_to_activate(stage) {}
    void finish(int r) {
      continuation->continue_function(r, stage_to_activate);
    }
  };
  protected:
  virtual bool _continue_function(int r, int stage) = 0;
  private:
  void continue_function(int r, int stage) {
    bool done = _continue_function(r, stage);
    if (parallel_mode) {
      assert (!done);
      int deleted = parallel_stages_in_flight.erase(stage);
      assert(deleted);
      if (parallel_stages_in_flight.empty()) {
        r = 0;
        parallel_mode = false;
        done = _continue_function(r, serial_stage);
      }
    }
    if (done) {
      on_finish->complete(rval);
      on_finish = NULL;
      delete this;
      return;
    }
  }

public:
  Continuation(Context *c) :
    parallel_mode(false), parallel_failed(false),
    serial_stage(0), rval(0), on_finish(c) {}
  virtual ~Continuation() { assert(on_finish == NULL); }

  void begin() { continue_function(0, 0); }

  Context *get_serial_callback() {
    assert(!parallel_mode);
    ++serial_stage;
    return new Callback(this, serial_stage);
  }
  int get_serial_stage() { return serial_stage; }
  void skip_serial_stage(int skip_stage) {
    assert(skip_stage == ++serial_stage);
  }
  void set_rval(int new_rval) { rval = new_rval; }

  void go_parallel_until(int cleanup_stage) {
    parallel_mode = true;
    serial_stage = cleanup_stage;
  }

  Context *get_parallel_callback(int stage) {
    pair<std::set<int>::iterator,bool> retval =
        parallel_stages_in_flight.insert(stage);
    assert(!retval.second); // we'd better not have run it twice!
    return new Callback(this, stage);
  }

  void set_parallel_failed() { parallel_failed = true; }
  bool is_failed() { return parallel_failed; }
};
