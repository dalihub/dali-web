# P3 GPU 메모리·reflection 실패 증거

2026-11-05. [결과와 한계](../../P3-GPU-MEMORY-RESULTS.md).

- `headers/sw-result`: 각 두 프레임 115개 검사, 46개 GPU buffer/uniform readback, 실제 DALi 두 색 픽셀. 빈 actual 배열은 CPU 반환값·객체/참조 수명 검사이며 GPU readback으로 세지 않는다. GPU 배열은 actual/expected 전체를 비교한다.
- `reflection-before-result`: 실제 GL_INVALID_ENUM을 reflection 중 발생시켰을 때 Wasm index out of bounds, stopped 없음. 같은 오류의 수정 후 `reflection-result`는 failure→stopped다. `shader-result`는 실제 shader 문법 오류다. 진단 문자열이 있는 실패 검사는 오류가 없는 정상 성공과 구분한다.
- `production-result`: 계측 없는 host, 오류 없는 ready→stopped. 자체 픽셀/readback 검사는 없다.
- `*-verdict`, `*-inputs`: 판정과 실제 browser 입력 hash. `source-hashes`는 최종 생산 입력499개, `core-reuse`는 별도 최종 빌드의 같은3.1.24 Core archive 재사용과 Core 입력 drift0이다.
- `*-commands`: 명령별 종료 집계. source 검사 외 상세 argv·nm·map·compile_commands·브라우저 프로필·큰 로그는 ignored `logs/p3`·`build/p3`에 보존한다. SDK와 산출물은 이 디렉터리에 포함하지 않는다.
- `native-commands`: GNU11.4 및 Emscripten 각각 관련14개 compile/nm0, native configure3개0. `native-build-commands`는 전체 Adaptor 빌드3개 실패1(설치된 Core 헤더 부재)이다. runtime 성공 근거가 아니다.
- `validation`: HEAD/원격 시작 상태, 기존748개 lock/산출물 보존, 최종499개 생산 입력 drift0.

최종 경로는 `logs/p3/gpu-memory-final-002`, `browser-memory-{headers,sw}-final-001`, `browser-{shader,reflection}-headers-final-001`, `production-memory-final-001`, `browser-production-memory-final-001`, `native-memory-002`, `native-build-memory-001`이다. Core333 빌드는 `gpu-memory-final-001`이며 이 실행의 host header hash drift가 있어 **생산 전체 최종 근거로 사용하지 않는다**. 마지막 header를 고정하고 final-002에서 생산35개 모두 다시 compile/nm/link했다. Core archive 및 Core 소스는 변경되지 않았다.

재현은 [BUILDING](../../BUILDING.md)의3.1.24 선택과 GPU 메모리 절차를 따른다. readback은 동기화를 유발하므로 성능·일반 비동기 처리 비용의 증거가 아니다. Firefox 로컬 WebGL 실행이며 물리 GPU 가속 여부·Chromium·Pages·전체 종료/재생성·사용자 코드 격리를 판정하지 않는다.
