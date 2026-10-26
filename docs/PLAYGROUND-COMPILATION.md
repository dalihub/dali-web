# 서버 없는 플레이그라운드 컴파일 방향

2026-10-26 사용자 승인 반영. **원본 Emception 3.1.24와 같은 릴리스 계열 SDK로 미리 빌드한 DALi를 사용한다.** 별도 운영 컴파일 서버 없이 GitHub Pages에서 정적 파일을 제공하고 사용자 C++는 브라우저에서 컴파일·링크한다. 후보 선택의 재승인은 필요 없다.

## 실행 방식과 버전 계약

1. 개발 환경/CI에서 DALi Core·생산 GLES·host를 Emscripten 3.1.24로 미리 빌드한다. 컴파일러 Wasm·헤더·라이브러리·JS runtime·Worker 자산을 버전별 정적 패키지로 배포한다.
2. 브라우저의 컴파일 Worker에서 사용자 C++를 컴파일하고 준비된 DALi와 링크해 JS/Wasm/Worker 산출물을 만든다. Pages와 Service Worker는 파일 제공·캐시·격리 bootstrap을 담당하며 C++를 서버에서 컴파일하지 않는다. Actions도 실시간 사용자 컴파일 서버가 아니다.
3. 원본 Emception과 DALi의 SDK·libc++·링크 옵션을 함께 고정한다. 6.0.13 DALi 객체와 3.1.24 런타임을 섞지 않는다. SDK를 올리려면 양쪽 재빌드와 전체 strict 링크·실행 검증을 다시 한다. 릴리스 계열 정렬을 확인했으며 두 컴파일러 바이너리가 동일하다는 의미는 아니다.
4. 기본 WebGL2, FULL_ES3=0, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, pthread/OffscreenCanvas와 main 비차단 조건을 유지한다. 공개 Graphics API 변경·빈 GL/sync·undefined 억제로 성공 처리하지 않는다.
5. 컴파일 Worker와 사용자 코드 실행 격리는 별도 문제다. 기본 DALi host는 main 이벤트/DOM + pthread 업데이트·업로드·렌더다. 플레이그라운드 이벤트 Worker 가능성은 P3/P4 조기 게이트에서 검증한다. 무한 루프 응답성, 전체 pthread/Canvas 종료·재생성 검증 없이 격리를 완료했다고 하지 않는다.

## 채택 근거와 한계

[정렬 결과](EMCEPTION-3.1.24-ALIGNMENT.md)와 [증거](evidence/emception-3.1.24-alignment/README.md): Core 333단계·생산/host 35개 compile/nm·로컬 strict 링크 성공, 브라우저 전체 생산 링크와 새 사용자 C++ 컴파일/링크 성공, Vector2·문자열 unordered_map 함수 반환 5. 직접 헤더/SW 모두 공유 메모리 모듈 로딩·함수 실행 성공이다.

`dali_web_start`는 호출하지 않았다. 실제 DALi 프레임·Worker GL·GPU 보존/readback·종료/재생성·실제 Pages는 미검증이다. P2 링크 마일스톤 완료를 유지하되 P3/P4·P8/P9 완료로 확대하지 않는다. GameGuild 4.5.0/emsdk 5.0.7의 JS 생성 결함은 별도 후보 실패이며 채택 대상이 아니다.

## 기존 lock과 다음 작업

후속 구현: [P3 초기 결과](P3-INITIAL-RESULTS.md)에서 전용3.1.24 lock/선택·CPU18/5·실제 사각형 픽셀과 로컬 헤더/SW 비교를 완료했다. 아래 문서-only 설명은 승인 시점의 기록이다.

이번 작업은 문서 갱신이다. 기존 `toolchain.lock.json`의 6.0.13은 과거 재현용으로 보존했고 자동 도구 선택을 변경하지 않았다. **다음 작업 기준은 승인된 3.1.24**다. 다음 컨텍스트에서 기존 lock을 덮어쓰지 않는 별도 3.1.24 프로필/lock과 명시적인 도구 선택을 정리한다. 패키지에는 실제 도구 버전·release/archive hash·컴파일러 자산 hash·DALi revision/dirty source manifest·링크 옵션을 연결해야 한다. 정적 패키징·라이선스 고지·캐시 업데이트·오류 표시·편집 UI는 P8/P9의 남은 작업이다.

다음 실행 순서는 [HANDOFF](HANDOFF.md)와 [다음 요청문](NEXT-CONTEXT.md)을 따른다. 3.1.24에서 기존 Controller 18모드/wrapper 5모드를 재검증하고 P3 실제 DALi 초기화·픽셀·GPU 보존으로 진행한다. 이미 성공한 컴파일러 후보 탐색을 반복하지 않는다.

GitHub Pages+SW 우선이며 실제 Pages 검증·배포는 별도 승인이다. 실패/과도한 비용이면 직접 호스팅을 검토하고 다른 관리형 호스팅은 사용하지 않는다. 로컬 loopback 파일 제공을 위한 임시 검증 서버는 운영 컴파일 서버 도입과 다르다. 현재 운영 서버 구축·upstream 갱신·배포는 하지 않는다.
