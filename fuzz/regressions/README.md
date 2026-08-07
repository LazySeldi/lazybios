# Fuzz regression inputs

Inputs that crashed the library before a fix. Each one is checked in so the fix
stays fixed: they are copied into the seed corpora by `make_corpus.sh`, and
`run_regressions.sh` replays every one of them directly.

```sh
fuzz/run_regressions.sh build-fuzz
```

| Input | Was |
| --- | --- |
| `entry_point/truncated-sm-entry` | 7-byte `_SM_` entry point accepted by `lazybiosParseEntry`, overlaid with the 31-byte layout: heap-buffer-overflow in `lazybiosPrintSMVer` |
| `decoders/type9-characteristics-overflow` | `lazybiosType9Characteristics1Str` accumulating `snprintf`'s return value past `buf_len`: heap-buffer-overflow writing `buf[len - 2]` |

Add new ones under the directory named after the target that found them.
