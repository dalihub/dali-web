# P2-b 최종 생산 증거

2026-10-23. production-003의 실제 Core·생산 Controller·자원·명령·D3 host 기본 WebGL2 strict 링크는 exit 0이다. [결과/개별 종료 코드](result.json), [전체 명령](production-003-commands.json), [source manifest](source-manifest.json), [입력 hash](source-hashes.json), [nm 경계](nm-web-boundary.json), [실제 map 발췌](map-excerpt.txt)를 보존한다. SDK·도구 버전은 version 로그를 따른다. 개인 경로는 환경변수 표기로 치환했다.

Core 333개 빌드 단계(331개 compilation entry와 생성·archive 단계)와 생산/host 35개 객체를 사용했다. FULL_ES3=0, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, pthread이며 테스트 fixture·linker wrap·빈 GL/EGL/sync를 생산 링크에 넣지 않았다. 전체 map과 로그는 logs/p2-b/production-003에 남겼고 대용량 파일은 추적하지 않는다. map의 크기와 SHA256은 result.json에 있다.

controller-007은 실제 생산 Buffer/Controller와 Core uniform의 GL 주입 CPU 검사 18모드이다. gl-unit-002는 실제 wrapper의 GL 주입 CPU 검사 5모드이다. 테스트의 미사용 GL 호출은 abort한다. 이들 CPU 결과는 실제 GPU 보존·픽셀·브라우저 수명 검증이 아니다. 과거 FULL_ES3 비교 링크도 runtime 근거가 아니다.

production-sources-001은 GLES/VULKAN/DYNAMIC source 회귀, closure-002는 정식 Web 목록과 서로 독립적인 native generated source 경로 검사다. production-native-002의 native configure 3개는 compiler 부재로 exit 1, 추출/공통 13개 Emscripten compile/nm은 모두 0이다. native build/install/runtime 성공으로 간주하지 않는다. 각 실행 명령 파일에는 예상 거부 검사도 실제 종료 코드 그대로 기록했다.

최종 입력 hash 일치와 세션 시작 당시 Core·기존 evidence·lock 보존을 검사했다. 최초 Controller checkpoint와 과거 실패 기록은 보존했다. 구현·특이 사항·미검증 영향 범위와 P3/P4 게이트는 [결과 문서](../../P2-b-PRODUCTION-RESULTS.md)를 따른다. 브라우저 실행, 서버 구축, Pages 배포는 수행하지 않았다.
