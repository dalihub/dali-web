# 원본 Emception 3.1.24 정렬 증거

[판정과 한계](../../EMCEPTION-3.1.24-ALIGNMENT.md). 로컬 SDK 빌드와 브라우저 컴파일·링크를 구별한다.

- `native-commands.json`, `version-*.log`: 최종 로컬 configure/Core333·생산35 compile/nm·strict 링크의 실제 명령과 종료 코드.
- `source-manifest.json`: Core와 생산 host의 compile entry 목록. 전체 compile_commands와 각 nm 로그는 ignored 원본 경로, 파일 hash는 validation에 기록한다.
- `browser-inputs.json`: 브라우저에 제공한 새 35객체와 Core archive의 hash·크기. 기존 6.0.13 산출물을 섞지 않았다.
- `browser-tests.json`: 브라우저 em++ 실제 버전, 생산 링크, 사용자 소스·명령·개별 returncode, Wasm imports/exports, 산출물 hash·크기, 실제 사용자 함수 반환값5.
- `symbol-check.json`: 새 Core의 hash_memory 참조0. 이전 참조20은 원본 Emception assessment-006 증거와 비교한다.
- `map-excerpts.json`: 브라우저 map의 실제 Controller·Core·WebBufferMemory·host·사용자 함수 발췌. 전체 map은 각각 약17MB여서 ignored 로그에 보존한다.
- `runtime-headers.json`, `runtime-sw.json`: 같은 생성 모듈의 직접 헤더 및 헤더 없는 SW 비교. 공유 메모리·함수 실행 증거이며 DALi 렌더 증거가 아니다.
- `source-commands.json`: GLES/VULKAN/DYNAMIC source 회귀 및 예상 거부 명령 종료 코드.
- `validation.json`: 초기 실패/최종 성공 구별, 산출물 hash, 기존 생산 입력497개 보존, 미검증 게이트.

`env.sh`와 `probe-*.py`는 실제 실행 스크립트의 개인 경로 정규화 사본이다. `${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK_3124}`, `${DALI_WEB_HOST_TOOLS}`, `${DALI_WEB_NODE}`를 자신의 경로로 치환해야 하며 자동 설치 스크립트가 아니다. host Node 버전은 24.19.0이었다. 기존 scripts/check-web-production.py를 새 build/run 경로와 고정 SDK env로 실행한다. SDK config·캐시를 진행 중 변경하지 않는다.

브라우저 재현에는 기존 원본 Emception 공개 자산을 local assets에 준비하고 `production/`에 browser-inputs의 새 객체를 배치한다. 기존 [Emception 증거](../emception/README.md)의 자산 준비·외부 차단 조건을 따른다. `probe-aligned.py`는 WebDriver로 오직 loopback 파일을 전달하며 컴파일과 링크는 브라우저 Wasm 도구에서 수행한다. 내보낸 JS/Wasm/Worker/map의 SHA256은 브라우저 결과와 로컬 파일을 대조했다.

runtime-check와 SW 파일은 이번 최소 로딩 검사 전용이다. `aligned-output/`을 생성 산출물 디렉터리에 연결하고 `probe-runtime.py headers`, `probe-runtime.py sw`를 각각 새 프로필/로그에서 실행한다. SW는 네트워크 응답에 격리 헤더를 붙이는 검사용이며 배포·캐시·업데이트 정책 구현이 아니다. 전체 운영 서버 없이 로컬 정적 파일 제공만 했다.

전체 로컬 자료: `tools/emsdk-3.1.24`, `build/emception/alignment-3124-{001,002}`, `logs/emception/alignment-3124-*`. 최초 SDK tar.xz 404 뒤 공식 tbz2 다운로드, 첫 configure/cache 재초기화 실패도 보존한다. SDK·바이너리·프로필·대용량 로그는 추적하지 않는다. 브라우저 pool 초기화 이후 사용자 함수만 호출했으며 host 시작·GPU·Pages·전체 종료 인수를 완료하지 않았다.
