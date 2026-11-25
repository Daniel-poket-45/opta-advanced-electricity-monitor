-- 1. Reduz o tamanho da página de 4096 para 1024 bytes (casa melhor com os blocos do LittleFS e poupa RAM)
-- IMPORTANTE: Deve ser o PRIMEIRO comando antes de criar qualquer tabela!
PRAGMA page_size = 1024;

-- 2. Obriga tabelas e índices temporários a ficarem na RAM (o LittleFS do Opta não tem pasta /tmp)
PRAGMA temp_store = MEMORY;

-- 3. Substitui o WAL (que exige mmap/shared-memory ausentes no LittleFS) por TRUNCATE
PRAGMA journal_mode = TRUNCATE;

-- 4. Limita o cache do SQLite para ~100 KB de RAM
PRAGMA cache_size = -100;

-- 5. Habilita o suporte a chaves estrangeiras
PRAGMA foreign_keys = ON;

-- Declaração de colunas da tabela medidores
CREATE TABLE IF NOT EXISTS medidores (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    modbus_id INTEGER NOT NULL UNIQUE,
    numero_de_serie TEXT,
    localizacao TEXT
);

-- Declaração de colunas da tabela dados_armazenados
CREATE TABLE IF NOT EXISTS dados_armazenados (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    medidor_id INTEGER NOT NULL,
    -- Armazena no formato ISO8601 ('YYYY-MM-DD HH:MM:SS') pra consultar rápido
    timestamp TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    
    -- Tensões (V)
    tensao_l1 REAL,  -- Tensão da fase 1 em relação a 0vca
    tensao_l2 REAL,  -- Tensão da fase 2 em relação a 0vca
    tensao_l3 REAL,  -- Tensão da fase 3 em relação a 0vca
    tensao_l12 REAL, -- Tensão da fase 1 em relação a fase 2
    tensao_l23 REAL, -- Tensão da fase 2 em relação a fase 3
    tensao_l31 REAL, -- Tensão da fase 3 em relação a fase 1
    
    -- Correntes (A)
    corrente_l1 REAL, -- Corrente da fase 1
    corrente_l2 REAL, -- Corrente da fase 2
    corrente_l3 REAL, -- Corrente da fase 3
    
    -- Potências e Fator de Potência
    potencia_ativa_total REAL,      -- W ou kW
    potencia_ativa_l1 REAL,         -- Potência da fase 1
    potencia_ativa_l2 REAL,         -- Potência da fase 2
    potencia_ativa_l3 REAL,         -- Potência da fase 3
    potencia_reativa_total REAL,    -- var
    potencia_aparente_total REAL,   -- VA
    fator_potencia_total REAL,      -- cos(phi)
    frequencia REAL,                -- Hz

    -- Distorção Harmônica Total (%)
    thd_tensao_l1 REAL,             -- THDU da tensão da fase 1 (%)
    thd_tensao_l2 REAL,             -- THDU da tensão da fase 2 (%)
    thd_tensao_l3 REAL,             -- THDU da tensão da fase 3 (%)
    thd_corrente_l1 REAL,           -- THDI da corrente da fase 1 (%)
    thd_corrente_l2 REAL,           -- THDI da corrente da fase 2 (%)
    thd_corrente_l3 REAL,           -- THDI da corrente da fase 3 (%)
    
    -- Totalizadores de Energia
    energia_ativa_importada REAL,   -- kWh
    energia_ativa_exportada REAL,   -- kWh
    energia_reativa_total REAL,     -- kvarh
    tarifa_ativa INTEGER,           -- Coeficiente de tarifa ativa
    
    -- Relação com medidores
    FOREIGN KEY (medidor_id) REFERENCES medidores(id) ON DELETE CASCADE
);

-- Índice composto essencial para consultas rápidas por medidor e período
CREATE INDEX IF NOT EXISTS idx_leituras_medidor_tempo 
ON dados_armazenados (medidor_id, timestamp);
