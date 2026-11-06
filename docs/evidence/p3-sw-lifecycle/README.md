# P3 SW 수명주기 검사 증거

2026-11-06. [결과·중단 원인·다음 제안](../../P3-SW-LIFECYCLE-RESULTS.md).

- `runtime-result.json`: 실제 Firefox의 순서별 관측. 최초·재방문·업데이트 후·503 업데이트 실패 후 각각 실제 DALi 두 프레임과 ready→stopped. 해제 뒤 재방문2회는 같은 오류이며 성공 집계에 포함하지 않는다.
- `verdict.json`: 전체 SW 게이트 실패, exit1, D4 중단.
- `input-hashes.json`: 기존 fixture 및 동일3.1.24 E runtime 입력. 업데이트는 served SW 복사본 끝에 주석 한 줄만 추가했다. 원본 SW는 변경하지 않았다.
- `validation.json`: 개별 관측의 독립 검산, 브라우저 버전·보존 hash·실행 harness 및 업데이트 복사본 hash. 관측 검산 성공과 SW 인수 실패는 별개다.

재현: BUILDING의 P3 SW 수명주기 절차. 전체 로그·HTTP503/404·프로필은 `logs/p3/sw-lifecycle-002`, 초기 socket 권한 실패는 `sw-lifecycle-001`에 보존한다. 001은 browser 실행 전 실패다. 외부 연결 차단 프록시와 CSP, loopback 정적 파일 제공만 사용했다. 브라우저·driver·서버 종료 확인. 큰 runtime/프로필은 추적하지 않는다.

CacheStorage 부재는 HTTP cache 부재를 의미하지 않는다. 이번 로컬 서버의 Cache-Control 없음·registration.updateViaCache=imports를 관측했으며 Pages/CDN 응답·버전 혼용·오프라인 지원은 검증하지 않았다. 주석-only SW 업데이트는 앱 JS/Wasm/Worker 동시 버전 교체의 증거가 아니다.
