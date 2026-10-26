# P3 초기 브라우저 렌더링 결과

2026-10-26. **3.1.24 도구 선택·CPU18/5 재검증과 실제 DALi 사각형/배경의 두 프레임 픽셀, 로컬 직접 헤더/SW 비교를 완료했다. P3 전체는 진행 중이다.** [증거](evidence/p3-initial/README.md).

## 구현

1. `toolchain.emception-3.1.24.lock.json`과 `scripts/env-emception-3.1.24.sh`를 추가했다. 기존 6.0.13 lock/자동 탐색을 보존하고 SDK 경로·config/cache를 명시적으로 선택한다. compiler revision을 확인하고 생산 hash 기록에 선택 lock을 포함한다. SDK 설치·cache 재작성은 하지 않는다.
2. Web wrapper의 `UNIFORM_NAME_LENGTH`를 실제 active uniform 이름 조회로 변환한다. 이름의 NUL terminator를 포함하며 다른 uniform 조회는 기존 WebGL 경로를 유지한다. native wrapper·공개 Graphics API 변경 없음.
3. Core `UniformBufferV2::ReSpecifyCPU`는 크기 증가로 버퍼를 교체할 때만 기존 포인터를 무효화한다. 동일/작은/0-byte 요청에서는 유지 중인 CPU mapping을 다시 lock하지 않는다. 같은 크기→축소→0→복귀→증가 회귀 검사를 추가했다. 이 변경은 공통 Core 내부이므로 native에도 적용되며 native runtime은 미검증이다.
4. Node CPU 테스트는 `EXIT_RUNTIME=1`로 종료를 명시한다. 생산 browser runtime에는 이 옵션을 넣지 않는다.
5. 기본 OFF인 `DALI_WEB_TEST_READBACK`으로 GL owner에서 두 프레임의 두 픽셀을 읽고 FrameDone mailbox 뒤 main에서 결과를 수집한다. 실제 Controller/Core 렌더 결과를 읽으며 GL 주입·mock·빈 GL은 사용하지 않는다. 별도 브라우저 fixture는 `/probe/` 하위 경로에서 직접 헤더와 SW를 비교하고 오류·상태·픽셀·입력 hash·종료 판정을 기록한다.

## 검증

- `pixels-3124-002`: 새 Core333·생산/host35 compile/nm·strict 링크 각각0. FULL_ES3=0, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, pthread/OffscreenCanvas·main 비차단 유지. 최종 source 498개 hash drift 없음.
- `controller-3124-003`: 실제 Controller/Core uniform CPU18모드0. 수정 전 Core를 사용한 `controller-regression-001`은 추가된 반복 CPU uniform 검사에서 실패1, 수정 후 통과0.
- `gl-3124-003`: wrapper CPU5모드0. 각 모드에서 길이가 다른 uniform 이름과 역순 indices의 길이 조회도 검사했다.
- Firefox 157.0.1 `browser-pixels-headers-001`, `browser-pixels-sw-001`: 각각 종료0. 두 프레임 모두 배경 RGBA `[13,25,51,255]`, 사각형 `[229,76,25,255]`, GL error0, 오류 목록 없음, `ready → stopped`. 예상 색의 8-bit 반올림 차이는 ±1 허용이다.
- `production-final-001`은 최종 Core/생산 객체를 재사용하고 host만 readback define 없이 별도 컴파일·strict 링크했다. `browser-final-001` 직접 헤더 실행도 오류 없이 ready→stopped, 종료0이다. readback 동기화 없이도 정상 경로가 실행됨을 구별해 확인했다. 이 실행 자체의 픽셀 readback은 하지 않았다.
- SW는 첫 navigation isolation=false, 제어/reload 후 true·sharedMemory=true·controlled=true. 직접 헤더도 isolation/sharedMemory=true다. 모듈 로딩만 성공한 과거 검사와 달리 실제 pthread의 GL context/렌더/readback을 실행했다.
- GLES/VULKAN/DYNAMIC source 회귀·Web closure·서로 독립된 native generated cache 경로 통과. native configure3개는 C/C++ compiler 부재로1, native 추출/공통13개 Emscripten compile/nm은0이다.
- 보존 대상 기존 lock·3.1.24/6.0.13 산출물735개 hash 변경 없음. HEAD와 원격 main은 인계 기록과 동일. commit/push·upstream 갱신·배포·운영 서버 구축 없음. 임시 브라우저·driver·loopback 서버는 종료했다.

## 특이 사항

1. 첫 CPU 실행은 PASS 출력 뒤 pthread pool 때문에 종료하지 않았다. 중단한 `controller-3124-001`/`gl-3124-001`은 통과로 집계하지 않고 보존했다. 테스트 링크의 명시적 종료 옵션으로 해결했다.
2. 최초 실제 browser 실행은 ready 이후 GL_INVALID_ENUM(1280)과 uniform reflection의 Wasm trap을 보였다. 3.1.24 JS glue가 `GL_UNIFORM_NAME_LENGTH`를 그대로 WebGL `getActiveUniforms`에 넘겼고, 후속 조회가 latch로 생략되어 block index 해석이 잘못됐다. 이름 조회 호환 처리를 추가해 이 정상 렌더 경로의 오류를 해소했다. 임의 shader/GL 실패 전반의 reflection 안전성까지 검증한 것은 아니다.
3. 그 다음 실행은 CPU uniform 중복 lock 실패를 정상 failure→stopped로 전달했다. Core의 persistent mapping 재사용 수정이 필요했다. 중복 lock을 허용하거나 실패 latch를 제거하지 않았다.
4. 중간 `pixels-3124-001` 빌드 중 Core 수정이 발생했으므로 최종 소스 재현 근거로 사용하지 않는다. 별도 `pixels-3124-002`를 처음부터 빌드해 최종 입력을 고정했다. 실패/중간 로그는 삭제하지 않았다.
5. 최초 sandbox 실행은 loopback socket 권한으로 실패했다. 승인된 로컬 실행 범위의 sandbox 확장으로 재실행했다. 자동 승인 거부나 사용자 승인 대기 상태는 아니다.

## 남은 게이트

P3의 첫 실제 렌더링 단위만 완료했다. 16-byte 패턴 중간4-byte·callback0/부분/전체·nonzero map/lock offset·CPU/GPU uniform·grow/recycle·수명의 종합 GPU readback 검사는 남는다. 이번 두 프레임의 단색 사각형과 CPU18 검사는 그 대체 근거가 아니다. readback은 GPU 동기화를 유발하므로 성능이나 일반 비동기 실행과 동일하다고 해석하지 않는다.

정상 stopped 통지를 실제로 확인했지만 전체 worker/자원 누수·실패 후 새 Module/Canvas 재생성·무한 루프 대응·context loss는 아직 인수하지 않았다. 이벤트 Worker 플레이그라운드 격리 조기 검사, SW 재방문·업데이트·해제/실패·캐시 정책, 실제 Pages 승인 게이트, Chromium과 native build/install/runtime도 남는다. 서버 없는 원본 Emception 방향과 P3/P4/P8/P9의 나머지 범위를 유지한다.
