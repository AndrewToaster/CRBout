STD += -std=c11

define _PS
	pwsh -NoProfile -command '$(1)'
endef

define mkdir
	$(call _PS,try { New-Item -ItemType Directory -Force -ErrorAction Stop -Path $(1) } catch { $$LASTEXITCODE=0 })
endef

define rm
	$(call _PS,try { Remove-Item -Recurse -Force -ErrorAction Stop $(1) } catch { $$LASTEXITCODE=0 })
endef