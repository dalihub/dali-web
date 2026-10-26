# P2-b Web 메모리 중간 증거

2026-10-23. [구현·한계·다음 작업](../../P2-b-WEB-MEMORY-RESULTS.md). 전체 생산 링크·Controller 통합·GPU 보존 성공 증거가 아니다.

- `commands.json`, `versions.txt`, `sources.txt`, `source-hashes.json`: memory-web-003 Core333·37개 compile/nm, 최종 입력·도구·명령·개별 종료 코드. 전체 Core hash/compile_commands는 로컬 로그에 보존.
- `link-webgl2.txt`, `link-full-es3.txt`, `result.json`: strict 각각 exit 1, 미해결10/6개, map 없음. wrapper/cache 경계 미완료, 실제 host 없음.
- `nm-memory-boundaries.json`: 공통 Controller와 Web memory/storage/buffer/failure의 undefined 목록. 최종 생산 링크 성공 판정이 아니다.
- `unit-commands.json`, `unit-result.txt`: memory-unit-003 compile/link 및 Node12모드 각각0. 실제 저장소·매핑·latch 코드 + 테스트 전용 GL 주입. `--wrap`은 단위 테스트에만 있으며 생산 진단에는 없다. CPU/GPU uniform은 여기서는 CPU/GPU 저장소 분기만 검사했고 실제 Core uniform 통합은 미검증.
- `source-commands.json`: memory-sources-001 GLES/VULKAN/DYNAMIC source 비교·잘못된 환경 조합 거부, exit0.
- `native-commands.json`: memory-native-002 native configure3개1(host compiler 부재), 추출7+공통 Controller/Buffer·Memory2/3=11개 Emscripten compile/nm 각각0. native 실행 증거가 아니다.
- `session.json`, `validation.json`: HEAD/원격 읽기 조회, 기존 Core·증거·lock89개 hash 보존, 최종 boundary 입력 drift 없음, native 추출 Buffer 메서드/Memory2·3 원문 보존.

전체 로그는 `logs/p2-b/memory-{web-001,web-002,web-003,unit-001,unit-002,unit-003,native-001,native-002,sources-001}`와 `web-memory-session-001`; 산출물은 별도 `build/p2-b/`에 있다. 001 compile 실패와 중간 입력 실행도 덮어쓰지 않았다. 전체 Core 컴파일 로그·SDK·산출물·프로필은 commit하지 않는다.

공개 경로는 `${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK}`, `${DALI_WEB_HOST_TOOLS}`로 정규화했다. 재현 도구는 `scripts/env.sh`의 선택 변수/PATH를 사용하며 개인 절대 경로를 요구하지 않는다. 새 build/run 경로로 `scripts/check-gles-boundary.py`, 기존 pthread Core build를 `DALI_WEB_CORE_BUILD_DIR`로 지정해 `scripts/check-web-memory.py`, 별도 경로로 source/native 스크립트를 실행한다. 정확한 argv는 JSON을 따른다. 최대 -j6이다.

CPU 저장소 grow/recycle 세대 검사를 실제 Buffer factory·Controller queue·shutdown 통합 검사로 해석하지 않는다. 후속 실제 host 비동기 failure/stop·기본 생산 strict 링크, P3 GPU 보존, D3/D4·P3/P4 게이트를 유지한다.
