# libopeninv

## About this fork

Bug-fix fork of jsphuebner/libopeninv, the library half of [moltenmosfet/stm32-sine](https://github.com/moltenmosfet/stm32-sine). Fixes in this repo: PI controller integral resolution, CAN receive filters, scheduler overrun resync, SDO frame length checks. None of it is dyno-specific.

`dyno-main` is the integration branch, created off the pinned baseline commit 78e3f72. Fix branches (`fix/T<nn>-…`) merge into it.

Most fixes are staged for upstream as a PR series. Per-change notes: the superproject's [FORK_NOTES.md](https://github.com/moltenmosfet/stm32-sine/blob/dyno-main/FORK_NOTES.md).

Nothing here is bench-validated on hardware yet. Changes are host-tested and compile-checked only.

[![Build status](../../actions/workflows/CI-build.yml/badge.svg)](../../actions/workflows/CI-build.yml)

Generic modules that can be used in many projects
