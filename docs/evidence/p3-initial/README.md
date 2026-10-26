# P3 최초 실제 렌더링 증거

[결과와 한계](../../P3-INITIAL-RESULTS.md). 최종 기준은 `logs/p3/pixels-3124-002`, `controller-3124-003`, `gl-3124-003`, `browser-pixels-{headers,sw}-001`이다.

- production-commands/source-hashes/collection: Core333·35객체·strict 링크 명령/종료/입력. readback 테스트 define을 포함한다. 전체 nm/map/compile_commands는 ignored logs/build에 보존.
- controller-commands: 최종18모드. controller-regression-failure는 수정 전 Core로 추가 회귀 실패를 재현한 기록. controller-before-fix는 기존18모드만으로 문제를 찾지 못한 앞선 성공 기록.
- wrapper-commands: 이름 길이 호환 처리까지 포함한5모드.
- browser-headers/sw, *-verdict, *-input-hashes: 실제 두 프레임 RGBA·GL 오류·ready/stopped·격리와 입력. SW는 `/probe/` 하위 경로의 첫 제어/reload다. 운영 캐시·Pages 증거가 아니다.
- browser-original-failure/trace-failure/uniform-lock-failure: 오류를 숨기지 않고 첫 GL 오류/trap과 다음 중복 lock 실패를 보존. trace는 JS GL 반환값을 바꾸지 않는 테스트 전용 추적과 debug link를 사용했다.
- production-final-commands/browser-final*: 동일 최종 Core/생산 객체와 계측 없는 host의 별도 링크·직접 헤더 실행. 오류 없이 ready→stopped, 픽셀 수집 없음.
- source/closure/native-commands: source 회귀와 native compile 한계. native configure1을 성공으로 해석하지 않는다.
- preservation/validation/version-*: 보존735개, 최종 생산 입력 drift0, 실제 compiler/Node/browser 버전과 미검증 게이트.

재현은 [BUILDING](../../BUILDING.md)의 명시 3.1.24 선택 후 새 build/log를 사용한다. 모든 SDK/산출물/큰 로그/브라우저 프로필은 commit 대상에서 제외한다. 작은 사본의 개인 경로는 환경변수로 정규화했다. 전체 사용자 코드 격리·GPU 부분 갱신/수명·종료/재생성 성공 증거는 아니다.
